#pragma once

class MyV8AccessorUserData;

class MyV8Accessor : public CefV8Accessor {
public:
    MyV8Accessor();

    virtual bool Get(const CefString& name,
        const CefRefPtr<CefV8Value> object,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception) OVERRIDE;

    virtual bool Set(const CefString& name,
        const CefRefPtr<CefV8Value> object,
        const CefRefPtr<CefV8Value> value,
        CefString& exception) OVERRIDE;


    CefRefPtr<CefV8Value> getObject(QString name)
    {
        CefRefPtr<CefV8Value> retval;
        if (m_values.contains(name))
        {
            retval = m_values.value(name);

        }
        return retval;
    }

    void setObject(QString name, CefRefPtr<CefV8Value> object)
    {
        m_values[name] = object;
    }

private:
    IMPLEMENT_REFCOUNTING(MyV8Accessor);

    QMap<QString, CefRefPtr<CefV8Value>> m_values;
};

class MyV8AccessorUserData : public CefBaseRefCounted
{
public:
    MyV8AccessorUserData(CefRefPtr<MyV8Accessor> accessor);

    CefRefPtr<MyV8Accessor>& accessor();

private:
    IMPLEMENT_REFCOUNTING(MyV8AccessorUserData);

    CefRefPtr<MyV8Accessor> m_accessor;
};

struct ObjectMetaInfo;
struct InjectApiMessage;

class QCefEventFunctionHandlerWrapper;
class QCefFuncCallback;
class AsyncCefMethodCallback;

class QCefRenderProcessHandler : public CefRenderProcessHandler
{
public:
    QCefRenderProcessHandler();

    long genFunctionId();
    void addFunctionCallback(CefRefPtr<CefFrame> frame, QString guid, const QCefFuncCallback& callback);

    QString saveAsyncMethodCallback(CefRefPtr<CefFrame> frame, std::shared_ptr<AsyncCefMethodCallback>);

    void invokeEvent(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message);
    void invokeCallback(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message);
    void runJavaScript(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message);
    void invokeJsFunction(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message);
    void invokeAsyncMethodCallback(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message);

    void prepareFrameHandler(CefRefPtr<CefFrame> frame);
    void releaeFrameHandler(CefRefPtr<CefFrame> frame);

    void injectApi(CefRefPtr<CefV8Context> context, QString parentPath, QString name, ObjectMetaInfo metaInfo);

    void injectProperty(CefRefPtr<CefV8Context> context, QString path, const ObjectMetaInfo& apiObject);

    void injectMethod(CefRefPtr<CefV8Context> context, QString path, const ObjectMetaInfo& apiObject);

    void injectEvent(CefRefPtr<CefV8Context> context, QString path, const ObjectMetaInfo& apiObject);

    CefRefPtr<CefV8Value> getOrCreateObject(CefRefPtr<CefV8Context> context, QString path);

    CefRefPtr<CefV8Value> getObject(CefRefPtr<CefV8Value> object, QString nodename);


    //CefRenderProcessHandler
    virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info);
    virtual void OnWebKitInitialized();

    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context);

    virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefV8Context> context);

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message);

private:
    IMPLEMENT_REFCOUNTING(QCefRenderProcessHandler);
    long m_funciontId = 0;
    bool m_contextReady = false;

    typedef QMap<QString, QCefEventFunctionHandlerWrapper> EventCallbackMap;

    QMap<int64_t, EventCallbackMap> m_frameEventCallbackHandlers;

    typedef QMap<QString, QCefFuncCallback>  FunctionCallbackMap;

    QMap<int64_t, FunctionCallbackMap> m_frameFuncionCallbacks;


    long m_callbackId = 0;
    typedef QMap<int, std::shared_ptr<AsyncCefMethodCallback>> AsyncMethodCallbackMap;

    QMap<int64_t, AsyncMethodCallbackMap> m_asyncCefCallbacks;

    std::vector<InjectApiMessage> m_apiMessages;
};