#pragma once


struct  CefProcessMessageWrapper
{
    CefProcessId pid;
    CefRefPtr<CefProcessMessage> processMessage;
};


class Q_DLL_QCEFCLIENT_API QCefJSDialogCallbackWrapper
{
public:
    CefRefPtr<CefJSDialogCallback> callback;
};

Q_DECLARE_METATYPE(QCefJSDialogCallbackWrapper);


class Q_DLL_QCEFCLIENT_API QCefBrowserWrapper
{
public:
    CefRefPtr<CefBrowser> browser;
};

Q_DECLARE_METATYPE(QCefBrowserWrapper);

class Q_DLL_QCEFCLIENT_API QCefListValueWrapper
{
public:
    CefRefPtr<CefListValue> listValue;
};

Q_DECLARE_METATYPE(QCefListValueWrapper);


inline QVariant converStringArg(CefRefPtr<QCefClientHandler> client, QString& arg)
{
    QVariant result;
    if (arg.startsWith("S_"))
    {
        result = arg.mid(2);
    }
    else if (arg.startsWith("F_"))
    {
        QString guid = arg;
        JsFunctionWrapper jsFunc(guid, client);
        result = QVariant::fromValue(jsFunc);
    }
    else if (arg.startsWith("O_"))
    {
        //json
        result = arg;
    }
    else
    {
        result = arg;
    }

    return result;
}

inline  QVariantList convertArgList(CefRefPtr<QCefClientHandler> client, CefRefPtr<CefListValue>& listValue)
{
    QVariantList variantArgList;
    for (int i = 0; i != listValue->GetSize(); ++i)
    {
        QVariant arg;
        CefValueType type = listValue->GetType(i);
        if (type == VTYPE_BOOL)
        {
            arg = listValue->GetBool(i);
        }
        else if (type == VTYPE_INT)
        {
            arg = listValue->GetInt(i);
        }
        else if (type == VTYPE_DOUBLE)
        {
            arg = listValue->GetDouble(i);
        }
        else if (type == VTYPE_STRING)
        {
            arg = converStringArg(client, QString::fromStdWString(listValue->GetString(i).ToWString()));
        }
        else if (type == VTYPE_LIST)
        {
            CefRefPtr<CefListValue> list = listValue->GetList(i);
            arg = convertArgList(client, list);
        }

        variantArgList.append(arg);
    }

    return variantArgList;
}


