#pragma once
#include "include/cef_v8.h"
#include "ceffunc.h"
#include "promise.h"

static QMap<int, QString> g_typeNames;



class AsyncCefMethodCallback
{
    CefRefPtr<CefV8Context> m_context;
    CefRefPtr<CefV8Value> m_resolve;
    CefRefPtr<CefV8Value> m_reject;
public:
    AsyncCefMethodCallback(const CefRefPtr<CefV8Context>& c, const CefRefPtr<CefV8Value>& res, const CefRefPtr<CefV8Value>& rej) {
        m_context = c;
        m_resolve = res;
        m_reject = rej;
    }

    void success(const CefRefPtr<CefListValue>& listValue)
    {
        if (m_resolve.get() && m_context.get() && m_context->Enter())
        {
            CefV8ValueList args;
            if (listValue->GetSize() == 0)
            {
                args.push_back(CefV8Value::CreateUndefined());
            }
            else
            {
                args.push_back(QCefFuncCallback::getCefV8Value(listValue, 0));
            }
            m_resolve->ExecuteFunction(nullptr, args);
            m_context->Exit();
        }
    }
    void fail(const CefString& exception)
    {
        if (m_reject.get() && m_context.get() && m_context->Enter())
        {
            CefV8ValueList args;
            args.push_back(CefV8Value::CreateString(exception));
            m_reject->ExecuteFunction(nullptr, args);
            m_context->Exit();
        }
    }
};

class JsFunctionWrapper : public JsFunctionDescriptor
{
public:
    JsFunctionWrapper(){ ; }
};


Q_DECLARE_METATYPE(JsFunctionWrapper)
Q_DECLARE_METATYPE(QJsonDocument)
Q_DECLARE_METATYPE(QFuture<QJsonDocument>)

inline void setupTypeNames()
{
    qRegisterMetaType<JsFunctionWrapper>("JsFunctionWrapper");
    qRegisterMetaType<QJsonDocument>("QJsonDocument");
    qRegisterMetaType<QFuture<QJsonDocument>>("QFuture<QJsonDocument>");

    g_typeNames.insert(QMetaType::QString, "string");
    g_typeNames.insert(QMetaType::Int, "int");
    g_typeNames.insert(QMetaType::QReal, "double");
    g_typeNames.insert(QMetaType::Float, "float");
    g_typeNames.insert(QMetaType::Bool, "bool");
    g_typeNames.insert(QMetaType::QVariantList, "array");

    g_typeNames.insert(QMetaTypeId<QJsonDocument>::qt_metatype_id(), "json");
    g_typeNames.insert(QMetaTypeId<JsFunctionWrapper>::qt_metatype_id(), "function");
}

inline QString getTypeName(int type)
{
    if (g_typeNames.contains(type))
    {
        return g_typeNames.value(type);
    }
    else
    {
        return "";
    }
}

class QCefRenderProcessHandler;
class QCefFunctionHandler : public CefV8Handler 
{
public:
    QString m_objectPath;
    QList<int> m_paramTypes;
    QCefRenderProcessHandler* m_renderHandler;
    CefRefPtr<CefFrame> m_frame;

    QCefFunctionHandler(QCefRenderProcessHandler* handler, CefRefPtr<CefFrame>& frame)
        : m_renderHandler(handler)
        , m_frame(frame)
    {

    }
    
    void setObjectPath(QString objectPath)
    {
        m_objectPath = objectPath;
    }

    void setParamTypes(const QList<QByteArray>& paramTypes)
    {
        m_paramTypes.clear();

        for (int index = 0; index != paramTypes.size(); ++index)
        {
            int type = QMetaType::type(paramTypes[index].constData());
            m_paramTypes.append(type);
        }
    }

    virtual bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception)
    {
        if (arguments.size() != m_paramTypes.size())
        {
            retval = CefV8Value::CreateBool(false);
            return true;
        }

        CefRefPtr<CefListValue> argList = CefListValue::Create();
        argList->SetSize(m_paramTypes.size());

        for (int index = 0; index != m_paramTypes.size(); ++index)
        {
            int paramType = m_paramTypes[index];

            CefRefPtr<CefV8Value> val = arguments.at(index);
            if (val->IsString())
            {
                if (paramType != QMetaType::QString)
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }
                QString arg = QString::fromStdWString(val->GetStringValue().ToWString());
                arg.prepend("S_");

                argList->SetString(index, CefString(arg.toStdWString().c_str()));
            }
            else if (val->IsInt())
            {
                if (paramType != QMetaType::Int && paramType != QMetaType::QReal)
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }

                argList->SetInt(index, val->GetIntValue());
            }
            else if (val->IsDouble())
            {
                if (paramType != QMetaType::QReal)
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }

                argList->SetDouble(index, val->GetDoubleValue() );
            }
            else if (val->IsBool())
            {
                if (paramType != QMetaType::Bool)
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }

                argList->SetBool(index, val->GetBoolValue());
            }
            else if (val->IsFunction())
            {
                if (paramType != QMetaTypeId<JsFunctionWrapper>::qt_metatype_id())
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }

                QString funcGuid = QString::number(m_renderHandler->genFunctionId());
                funcGuid.prepend("F_");
                argList->SetString(index, CefString(funcGuid.toStdWString().c_str()));

                QCefFuncCallback callback;
                callback.functionValue = val;
                callback.context = CefV8Context::GetCurrentContext();

                m_renderHandler->addFunctionCallback(m_frame, funcGuid, callback);
            }
            else if (val->IsArray())
            {
                if (paramType != QMetaType::QVariantList)
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }
         
                argList->SetList(index, convertCefV8Array2CefListValue(val));
            }
            else if (val->IsObject())
            {
                if (paramType != QMetaTypeId<QJsonDocument>::qt_metatype_id())
                {
                    exception = QString("wrong param type at position %1 should be %2").arg(index).arg(getTypeName(paramType)).toUtf8().constData();
                    retval = CefV8Value::CreateBool(false);
                    return true;
                }

                QString json = converCefObjectToJsonString(val);
                json.prepend("O_");
                argList->SetString(index, CefString(json.toStdWString().c_str()));
            }
        }


        auto context = CefV8Context::GetCurrentContext();
        auto window = context->GetGlobal();
        auto createPromise = window->GetValue(kPromiseCreatorFunction);
        auto promiseWrapper = createPromise->ExecuteFunctionWithContext(context, nullptr, CefV8ValueList());

        retval = promiseWrapper->GetValue("p");

        auto resolve = promiseWrapper->GetValue("resolve");
        auto reject = promiseWrapper->GetValue("reject");

        auto asyncCallback = std::make_shared<AsyncCefMethodCallback>(context, resolve, reject);
        auto callbackId = m_renderHandler->saveAsyncMethodCallback(m_frame, asyncCallback);

        CefRefPtr<CefBrowser> browser = m_frame->GetBrowser();
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("InvokeMethod");
        message->GetArgumentList()->SetSize(3);
        message->GetArgumentList()->SetString(0, m_objectPath.toUtf8().constData());
        message->GetArgumentList()->SetString(1, name);
        message->GetArgumentList()->SetList(2, argList);
        message->GetArgumentList()->SetString(3, callbackId.toStdString());
        browser->SendProcessMessage(PID_BROWSER, message);
        return true; 
    }


    CefRefPtr<CefListValue> convertCefV8Array2CefListValue(CefRefPtr<CefV8Value> val)
    {
        CefRefPtr<CefListValue> target = CefListValue::Create();
        int arrarLength = val->GetArrayLength();
        target->SetSize(arrarLength);
        for (int i = 0; i < arrarLength; ++i)
        {
            CefRefPtr<CefV8Value> value = val->GetValue(i);
            if (value->IsBool())
            {
                target->SetBool(i, value->GetBoolValue());
            }
            else if (value->IsInt() || value->IsUInt())
            {
                target->SetInt(i, value->GetIntValue());
            }
            else if (value->IsDouble())
            {
                target->SetDouble(i, value->GetDoubleValue());
            }
            else if (value->IsNull())
            {
                target->SetNull(i);
            }
            else if (value->IsString() || value->IsDate())
            {
                target->SetString(i, value->GetStringValue());
            }
            else if (value->IsArray())
            {
                CefRefPtr<CefListValue> newList = convertCefV8Array2CefListValue(value);
                target->SetList(i, newList);
            }
            else if (value->IsObject())
            {
                QString json = converCefObjectToJsonString(val);
                json.prepend("O_");
                target->SetString(i, CefString(json.toStdWString().c_str()));
            }
        }

        return target;
    }

	QVariant converCefObjectToVariant(CefRefPtr<CefV8Value> arg)
	{
		if (arg->IsString())
		{
			return QVariant::fromValue(QString::fromStdWString(arg->GetStringValue().ToWString()));
		}
		else if (arg->IsNull())
		{
			return QVariant();
		}
		else if (arg->IsDouble())
		{
			return QVariant(arg->GetDoubleValue());
		}
		else if (arg->IsBool())
		{
			return QVariant(arg->GetBoolValue());
		}
		else if (arg->IsArray())
		{
			QVariantList result;
			const auto count = arg->GetArrayLength();
			for (auto i = 0; i < count; i++)
			{
				result.append(converCefObjectToVariant(arg->GetValue(i)));
			}
			return result;
		}
		else
		{
			QVariantMap result;
			std::vector<CefString> keys;
			arg->GetKeys(keys);
			for (int i = 0; i != keys.size(); ++i)
			{
				CefString key = keys[i];

				CefRefPtr<CefV8Value> val = arg->GetValue(key);
				if (!val->IsFunction())
				{
					result.insert(QString::fromStdWString(key.ToWString()), converCefObjectToVariant(val));
				}
			}

			return result;
		}
		
	}


    QString converCefObjectToJsonString(CefRefPtr<CefV8Value> arg)
    {
		QString json = QString::fromUtf8(QJsonDocument::fromVariant(converCefObjectToVariant(arg)).toJson());
		std::cout << json.toStdString() << std::endl;

        return json;
    }

private:
    IMPLEMENT_REFCOUNTING(QCefFunctionHandler);

};



