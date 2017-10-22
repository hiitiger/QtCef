#pragma once
#include "include/cef_v8.h"

class QCefEventFunctionHandler :  public CefV8Handler
{
public:
    QString m_eventPah;
    std::mutex m_callbackLock;
    QList<QCefFuncCallback> m_callbacks;

    void setEventPath(QString eventPah)
    {
        m_eventPah = eventPah;
    }

    void invokeCallback(CefRefPtr<CefListValue>& message)
    {
        std::lock_guard<std::mutex> lock(m_callbackLock);
        for (int i = 0; i != m_callbacks.size(); ++i)
        {
            m_callbacks[i].execute(message);
        }
    }

    virtual bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception)
    {
        if (arguments.size() != 1)
        {
            exception = "wrong arguments size";
            return true;
        }

        if (name == "addListener")
        {
            std::lock_guard<std::mutex> lock(m_callbackLock);

            if (arguments[0]->IsFunction())
            {
                QCefFuncCallback callback;
                callback.context = CefV8Context::GetCurrentContext();
                callback.functionValue = arguments[0];
                m_callbacks.append(callback);

                CefRefPtr<CefBrowser> browser = CefV8Context::GetCurrentContext()->GetBrowser();
                CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("AddListner");
                message->GetArgumentList()->SetSize(1);
                message->GetArgumentList()->SetString(0, m_eventPah.toUtf8().constData());
                browser->SendProcessMessage(PID_BROWSER, message);
                retval = CefV8Value::CreateBool(true);
            }
        }
        else if (name == "removeListener")
        {
            std::lock_guard<std::mutex> lock(m_callbackLock);

            for (int i = 0; i != m_callbacks.size(); ++i)
            {
                if (m_callbacks[i].functionValue->IsSame(arguments[0]))
                {
                    m_callbacks.removeAt(i);
                    retval = CefV8Value::CreateBool(true);
                    break;
                }
            }
        }

        return true;
    }

private:
    IMPLEMENT_REFCOUNTING(QCefEventFunctionHandler);
};

class  QCefEventFunctionHandlerWrapper
{
public:
    CefRefPtr<QCefEventFunctionHandler> handler;
};

