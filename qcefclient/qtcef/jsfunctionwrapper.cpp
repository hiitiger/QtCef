#include "stable.h"

#include "jsfunctionwrapper.h"

Q_DECLARE_METATYPE(QJsonDocument)



JsFunctionWrapper::JsFunctionWrapper(QString guid, CefRefPtr<QCefClientHandler> client)
    : JsFunctionDescriptor(guid)
    , m_client(client)
{

}

void JsFunctionWrapper::invoke(QVariantList argument)
{
    if (m_client)
    {
        CefRefPtr<CefBrowser> browser = m_client->GetBrowser();
        if (browser)
        {
            CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("InvokeCallback");
            message->GetArgumentList()->SetSize(2);
            message->GetArgumentList()->SetString(0, funcGuid().toUtf8().constData());
            message->GetArgumentList()->SetList(1, convertToCefList(argument));

            browser->SendProcessMessage(PID_RENDERER, message);
        }
    }
}

CefRefPtr<CefListValue> JsFunctionWrapper::convertToCefList(QVariantList argument)
{
    CefRefPtr<CefListValue> argList = CefListValue::Create();
    argList->SetSize(argument.size());

    for (int i = 0; i != argument.size(); ++i)
    {
        int type = argument.at(i).userType();

        if (type == QVariant::String)
        {
            argList->SetString(i, argument.at(i).toString().prepend("S_").toStdWString());
        }
        else if (type == QVariant::Int)
        {
            argList->SetInt(i, argument.at(i).toInt());
        }
        else if (type == QVariant::Double)
        {
            argList->SetDouble(i, argument.at(i).toReal());
        }
        else if (type == QVariant::Bool)
        {
            argList->SetBool(i, argument.at(i).toBool());
        }     
        else if (type == QVariant::List)
        {
            QVariantList variantList = argument.at(i).toList();
            CefRefPtr<CefListValue> list = convertToCefList(variantList);
            argList->SetList(type, list);
        }
        else if (type == QMetaTypeId<QJsonDocument>::qt_metatype_id())
        {
            QJsonDocument doc = argument.at(i).value<QJsonDocument>();
            QString jsonString = QString::fromUtf8(doc.toJson().constData());
            jsonString.prepend("O_");
            argList->SetString(i, jsonString.toStdWString());
        }
        else
        {
            assert(0);
        }
    }

    return argList;
}

