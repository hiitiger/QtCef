#pragma once


class QCefFuncCallback
{
public:
    CefRefPtr<CefV8Context> context;
    CefRefPtr<CefV8Value> functionValue;

    void execute(CefRefPtr<CefListValue>& argument)
    {
        context->Enter();

        CefV8ValueList arguments;
        arguments.reserve(argument->GetSize());

        for (int i = 0; i != argument->GetSize(); ++i)
        {
            CefRefPtr<CefV8Value> args = getCefV8Value(argument, i);
            arguments.push_back(args);
        }

        CefRefPtr<CefV8Value> retval = functionValue->ExecuteFunction(NULL, arguments);
        if (retval.get())
        {
            if (retval->IsBool())
            {
                bool handled = retval->GetBoolValue();
                (void)handled;
            }
        }

        context->Exit();
    }

    static CefRefPtr<CefV8Value> getCefV8Value(const CefRefPtr<CefListValue>& listValue, int index)
    {
        CefRefPtr<CefV8Value> value;
        CefValueType type = listValue->GetType(index);
        if (type == VTYPE_NULL)
        {
            value = CefV8Value::CreateNull();
        }
        else if (type == VTYPE_INVALID)
        {
            value = CefV8Value::CreateUndefined();
        }
        else if (type == VTYPE_BOOL)
        {
            value = CefV8Value::CreateBool(listValue->GetBool(index));
        }
        else if (type == VTYPE_INT)
        {
            value = CefV8Value::CreateInt(listValue->GetInt(index));
        }
        else if (type == VTYPE_DOUBLE)
        {
            value = CefV8Value::CreateDouble(listValue->GetDouble(index));
        }
        else if (type == VTYPE_STRING)
        {
            QString str = QString::fromStdWString(listValue->GetString(index));
            if (str.startsWith("S_"))
            {
                str = str.mid(2);
                value = CefV8Value::CreateString(str.toStdWString());
            }
            else if (str.startsWith("O_"))
            {
                str = str.mid(2);
                value = converToCefObject(str);
            }
        }
        else if (type == VTYPE_LIST)
        {
            CefRefPtr<CefListValue> list = listValue->GetList(index);
            value = CefV8Value::CreateArray(list->GetSize());

            for (size_t i = 0; i < list->GetSize(); ++i)
            {
                value->SetValue(i, getCefV8Value(list, index));
            }
        }
        else
        {
            Q_ASSERT(false);
        }
        return value;
    }

    static CefRefPtr<CefV8Value> converToCefObject(const QJsonValue& jsonValue)
    {
        CefRefPtr<CefV8Value> value;

        if (jsonValue.isDouble())
        {
            value = CefV8Value::CreateDouble(jsonValue.toDouble());
        }
        else if (jsonValue.isBool())
        {
            value = CefV8Value::CreateBool(jsonValue.toBool());
        }
        else if (jsonValue.isString())
        {
            value = CefV8Value::CreateString(jsonValue.toString().toStdWString());
        }
        else if (jsonValue.isArray())
        {
            QJsonArray jsonArray = jsonValue.toArray();
            value = CefV8Value::CreateArray(jsonArray.size());

            for (int i = 0; i != jsonArray.size(); ++i)
            {
                CefRefPtr<CefV8Value> subValue = converToCefObject(jsonArray.at(i));
                value->SetValue(i, subValue);
            }
        }
        else if (jsonValue.isObject())
        {
            QJsonObject object = jsonValue.toObject();
            value = converToCefObject(object);
        }
		else if (jsonValue.isNull())
		{
			value = CefV8Value::CreateNull();
		}
		else if (jsonValue.isUndefined())
		{
			value = CefV8Value::CreateUndefined();
		}

        return value;
    }

    static CefRefPtr<CefV8Value> converToCefObject(const QJsonObject& jsonObject)
    {
        CefRefPtr<CefV8Value> value = CefV8Value::CreateObject(nullptr, nullptr);

        QStringList keys = jsonObject.keys();
        Q_FOREACH(QString key, keys)
        {
			std::cout << key.toStdString() << std::endl;
            CefRefPtr<CefV8Value> child = converToCefObject(jsonObject.value(key));
            value->SetValue(key.toStdWString(), child, V8_PROPERTY_ATTRIBUTE_NONE);
        }

        return value;
    }

    static CefRefPtr<CefV8Value> converToCefObject(QString jsonString)
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());

        QJsonObject root = jsonDoc.object();

        CefRefPtr<CefV8Value> value = converToCefObject(root);

        return value;
    }
};