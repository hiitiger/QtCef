#pragma once
#include "include/cef_v8.h"

static QMap<int, QString> g_typeNames;


class JsFunctionWrapper : public JsFunctionDescriptor
{
public:
    JsFunctionWrapper(){ ; }
};

Q_DECLARE_METATYPE(JsFunctionWrapper)

inline void setupTypeNames()
{
    qRegisterMetaType<QJsonDocument>("QJsonDocument");
    qRegisterMetaType<JsFunctionWrapper>("JsFunctionWrapper");

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


        CefRefPtr<CefBrowser> browser = m_frame->GetBrowser();
        CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("InvokeMethod");
        message->GetArgumentList()->SetSize(3);
        message->GetArgumentList()->SetString(0, m_objectPath.toUtf8().constData());
        message->GetArgumentList()->SetString(1, name);
        message->GetArgumentList()->SetList(2, argList);
        browser->SendProcessMessage(PID_BROWSER, message);
        retval = CefV8Value::CreateBool(true);
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

    QString converCefObjectToJsonString(CefRefPtr<CefV8Value> arg)
    {
        QString json;
        json.append ('{');
        std::vector<CefString> keys;
        arg->GetKeys(keys);

        for (int i = 0; i != keys.size(); ++i)
        {
            CefString key = keys[i];

            CefRefPtr<CefV8Value> val = arg->GetValue(key);
            if (!val->IsFunction())
            {
                json.append("\"");
                json.append(key.ToString().c_str());
                json.append("\":");
                json.append(converCefObjectToJsonString(val));
                json.append(",");
            }
        }

        if (keys.size() > 0)
        {
            json.remove(json.size() - 1, 1);
        }
        
        json.append('}');
        return json;
    }

private:
    IMPLEMENT_REFCOUNTING(QCefFunctionHandler);

};

