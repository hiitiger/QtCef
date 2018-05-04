#include "stable.h"
#include "qceffunctionhandler.h"
#include "qcefrenderprocesshandler.h"
#include "ceffunc.h"
#include "demoapi.h"

MyV8Accessor::MyV8Accessor()
{

}

bool MyV8Accessor::Get(const CefString& name, const CefRefPtr<CefV8Value> object, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
    QString objectName = QString::fromStdWString(name.ToWString());

    if (!m_values.contains(objectName))
    {
        exception = QString("Property %1 not found.").arg(objectName).toStdWString();
        return true;
    }

    retval = m_values.value(objectName);

    return true;
}

bool MyV8Accessor::Set(const CefString& name, const CefRefPtr<CefV8Value> object, const CefRefPtr<CefV8Value> value, CefString& exception)
{
    (void)name; (void)object; (void)value; (void)exception;
    return false;
}

MyV8AccessorUserData::MyV8AccessorUserData(CefRefPtr<MyV8Accessor> accessor)
    : m_accessor(accessor)
{

}

CefRefPtr<MyV8Accessor>& MyV8AccessorUserData::accessor()
{
    return m_accessor;
}

//////////////////////////////////////////////////////////////////////////


QCefRenderProcessHandler::QCefRenderProcessHandler()
    :m_funciontId(0)
{

}

long QCefRenderProcessHandler::genFunctionId()
{
    return InterlockedIncrement(&m_funciontId);
}

void QCefRenderProcessHandler::addFunctionCallback(CefRefPtr<CefFrame> frame, QString guid, const QCefFuncCallback& callback)
{
    int64_t frameId = frame->GetIdentifier();

    if (m_frameFuncionCallbacks.contains(frameId))
    {
        FunctionCallbackMap& callbackMap = m_frameFuncionCallbacks[frameId];
        callbackMap.insert(guid, callback);
    }
}

void QCefRenderProcessHandler::invokeEvent(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message)
{
    std::vector<int64_t> frameIds;
    browser->GetFrameIdentifiers(frameIds);

    QString eventPath = QString::fromStdWString(message->GetArgumentList()->GetString(0).ToWString());

    for (size_t i = 0; i < frameIds.size(); i++)
    {
        if (m_frameEventCallbackHandlers.contains(frameIds[i]))
        {
            EventCallbackMap& callbackMap = m_frameEventCallbackHandlers[frameIds[i]];
            
            if (callbackMap.contains(eventPath))
            {
                CefRefPtr<QCefEventFunctionHandler> handler = callbackMap[eventPath].handler;

                CefRefPtr<CefListValue> argument = message->GetArgumentList()->GetList(1);

                handler->invokeCallback(argument);
            }
        }
    }
}

void QCefRenderProcessHandler::invokeCallback(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message)
{
    std::vector<int64_t> frameIds;
    browser->GetFrameIdentifiers(frameIds);

    QString functionGuid = QString::fromStdWString(message->GetArgumentList()->GetString(0).ToWString());

    for (size_t i = 0; i < frameIds.size(); i++)
    {
        if (m_frameFuncionCallbacks.contains(frameIds[i]))
        {
            FunctionCallbackMap& callbackMap = m_frameFuncionCallbacks[frameIds[i]];

            if (callbackMap.contains(functionGuid))
            {
                QCefFuncCallback& callback = callbackMap[functionGuid];
                
                CefRefPtr<CefListValue> argument = message->GetArgumentList()->GetList(1);

                callback.execute(argument);
            }
        }
    }
}

void _runJavaScript(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message)
{
    CefString code = message->GetArgumentList()->GetString(0);
    browser->GetMainFrame()->ExecuteJavaScript(code, "", 0);
}

void QCefRenderProcessHandler::runJavaScript(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message)
{
    __try{
        _runJavaScript(browser, message);
    }
    __except (EXCEPTION_EXECUTE_HANDLER){

    }
}

void QCefRenderProcessHandler::invokeJsFunction(CefRefPtr<CefBrowser>& browser, CefRefPtr<CefProcessMessage>& message)
{
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
    CefRefPtr<CefV8Context> v8Context = frame->GetV8Context();
    CefString funName = message->GetArgumentList()->GetString(0);
    CefRefPtr<CefListValue>  argument = message->GetArgumentList()->GetList(1);

    if (v8Context.get())
    {
        v8Context->Enter();
        CefRefPtr<CefV8Value> globalObj = v8Context->GetGlobal();
        if (globalObj->HasValue(funName))
        {
            CefRefPtr<CefV8Value> evalFunc = globalObj->GetValue(funName);
            CefV8ValueList arguments;
            arguments.reserve(argument->GetSize());

            for (int i = 0; i != argument->GetSize(); ++i)
            {
                CefRefPtr<CefV8Value> args = QCefFuncCallback::getCefV8Value(argument, i);
                arguments.push_back(args);
            }

            evalFunc->ExecuteFunctionWithContext(v8Context, globalObj, arguments);
        }

        v8Context->Exit();
    }
}

void QCefRenderProcessHandler::prepareFrameHandler(CefRefPtr<CefFrame> frame)
{
    int64_t frameId = frame->GetIdentifier();
    if (m_frameEventCallbackHandlers.contains(frameId))
    {
        Q_ASSERT_X(false,  "QCefBrowser", "prepare rameEventCallbackHandlers while already has that id");
    }
    EventCallbackMap eventCallbackMap;
    m_frameEventCallbackHandlers.insert(frameId, eventCallbackMap);

    if (m_frameFuncionCallbacks.contains(frameId))
    {
        Q_ASSERT_X(false, "QCefBrowser", "prepare frameFuncionCallbacks while already has that id");
    }
    FunctionCallbackMap funtionCallbackMap;
    m_frameFuncionCallbacks.insert(frameId, funtionCallbackMap);
}

void QCefRenderProcessHandler::releaeFrameHandler(CefRefPtr<CefFrame> frame)
{
    int64_t frameId = frame->GetIdentifier();
    m_frameEventCallbackHandlers.remove(frameId);
    m_frameFuncionCallbacks.remove(frameId);
}

void QCefRenderProcessHandler::injectApi(CefRefPtr<CefV8Context> context, QString path, QObject* apiObject)
{
    context->Enter();

    CefRefPtr<CefV8Value> object = getOrCreateObject(context, path);

    injectProperty(context, path, apiObject);
    injectMethod(context, path, apiObject);
    injectEvent(context, path, apiObject);

    context->Exit();
}

void QCefRenderProcessHandler::injectProperty(CefRefPtr<CefV8Context> context, QString path, QObject* apiObject)
{
    CefRefPtr<CefV8Value> object = getOrCreateObject(context, path);

    const QMetaObject* metaObject = apiObject->metaObject();
  
    for (int i = 0; i < metaObject->propertyCount(); i++)
    {
        CefRefPtr<CefV8Value> value;

        QMetaProperty metaPorperty = metaObject->property(i);
        QString name = metaPorperty.name();

        if (metaPorperty.userType() == QMetaType::QString)
        {
            value = CefV8Value::CreateString(metaPorperty.read(apiObject).toString().toStdWString());
        }
        else if (metaPorperty.userType() == QMetaType::Int)
        {
            value = CefV8Value::CreateInt(metaPorperty.read(apiObject).toInt());
        }
        else if (metaPorperty.userType() == QMetaType::Double)
        {
            value = CefV8Value::CreateDouble(metaPorperty.read(apiObject).toDouble());
        }
        else if (metaPorperty.userType() == QMetaType::Bool)
        {
            value = CefV8Value::CreateBool(metaPorperty.read(apiObject).toBool());
        }

        object->SetValue(name.toStdWString(), value, V8_PROPERTY_ATTRIBUTE_NONE);
    }
}

void QCefRenderProcessHandler::injectMethod(CefRefPtr<CefV8Context> context, QString path, QObject* apiObject)
{
    CefRefPtr<CefV8Value> object = getOrCreateObject(context, path);

    const QMetaObject* metaObject = apiObject->metaObject();
  
    for (int i = 0; i != metaObject->methodCount(); ++i)
    {
        QMetaMethod metaMethod = metaObject->method(i);
        if (metaMethod.methodType() == QMetaMethod::Method)
        {
            QString methodName = QLatin1String(metaMethod.signature());
            methodName = methodName.left(methodName.indexOf("("));

            CefRefPtr<QCefFunctionHandler> handler = new QCefFunctionHandler(this, context->GetFrame());
            handler->setObjectPath(path);
            handler->setParamTypes(metaMethod.parameterTypes());

            CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction(methodName.toUtf8().constData(), handler);
            object->SetValue(methodName.toUtf8().constData(), func, V8_PROPERTY_ATTRIBUTE_NONE);
        }
    }
}

void QCefRenderProcessHandler::injectEvent(CefRefPtr<CefV8Context> context, QString path, QObject* apiObject)
{
    const QMetaObject* metaObject = apiObject->metaObject();
    QString injectName = QString::fromUtf8(metaObject->className()).toLower();

    for (int i = 0; i != metaObject->methodCount(); ++i)
    {
        QMetaMethod method = metaObject->method(i);
        if (method.methodType() == QMetaMethod::Signal)
        {
            QString eventName = QLatin1String(method.signature());
            eventName = eventName.left(eventName.indexOf("("));
            if (eventName == "destroyed")
            {
                continue;
            }

            QString eventPath = eventName;
            eventPath.prepend(".");
            eventPath.prepend(path);

            CefRefPtr<CefV8Value> eventObject = getOrCreateObject(context, eventPath);

            CefRefPtr<QCefEventFunctionHandler> eventHandler = new QCefEventFunctionHandler();

            eventHandler->setEventPath(eventPath);

            CefRefPtr<CefV8Value> funcAdd = CefV8Value::CreateFunction("addListener", eventHandler);
            eventObject->SetValue("addListener", funcAdd, V8_PROPERTY_ATTRIBUTE_NONE);

            CefRefPtr<CefV8Value> funcRemove = CefV8Value::CreateFunction("removeListener", eventHandler);
            eventObject->SetValue("removeListener", funcRemove, V8_PROPERTY_ATTRIBUTE_NONE);

            int64_t frameId = context->GetFrame()->GetIdentifier();

            if (m_frameEventCallbackHandlers.contains(frameId))
            {
                EventCallbackMap& callbacks = m_frameEventCallbackHandlers[frameId];
                if (!callbacks.contains(eventPath))
                {
                    QCefEventFunctionHandlerWrapper handlerWrapper;
                    handlerWrapper.handler = eventHandler;
                    callbacks.insert(eventPath, handlerWrapper);
                }
            }        
        }
    }
}

CefRefPtr<CefV8Value> QCefRenderProcessHandler::getOrCreateObject(CefRefPtr<CefV8Context> context, QString path)
{
    CefRefPtr<CefV8Value> global = context->GetGlobal();

    QStringList strList = path.split('.');

    CefRefPtr<CefV8Value> parentObject = global;
    CefRefPtr<CefV8Value> target;

    Q_FOREACH(QString name, strList)
    {
        target = getObject(parentObject, name);
        if (target == nullptr || target->IsUndefined())
        {
            CefRefPtr<MyV8Accessor> accessor = new MyV8Accessor();
            target = CefV8Value::CreateObject(accessor);
            target->SetUserData(new MyV8AccessorUserData(accessor));

            if (parentObject == global)
            {
                global->SetValue(name.toStdWString(), target, V8_PROPERTY_ATTRIBUTE_READONLY);
            }
            else
            {
                parentObject->SetValue(name.toUtf8().constData(), V8_ACCESS_CONTROL_DEFAULT, V8_PROPERTY_ATTRIBUTE_READONLY);
                MyV8AccessorUserData* accessorUserData = dynamic_cast<MyV8AccessorUserData*>(parentObject->GetUserData().get());
                if (accessorUserData)
                {
                    accessorUserData->accessor()->setObject(name, target);
                }
            }     
        }

        parentObject = target;
    }

    return parentObject;
}

CefRefPtr<CefV8Value> QCefRenderProcessHandler::getObject(CefRefPtr<CefV8Value> object, QString nodename)
{
    MyV8AccessorUserData* accessorUserData = dynamic_cast<MyV8AccessorUserData*>(object->GetUserData().get());
    if (accessorUserData)
    {
        return accessorUserData->accessor()->getObject(nodename);
    }
    else
    {
        return object->GetValue(nodename.toStdWString());
    }
}

LONG WINAPI TopLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    return EXCEPTION_EXECUTE_HANDLER;
}

void QCefRenderProcessHandler::OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info)
{
    SetUnhandledExceptionFilter(TopLevelExceptionHandler);
}

void QCefRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    prepareFrameHandler(frame);

    QString url = QString::fromStdWString(frame->GetURL().ToWString());
    QString host = QUrl(url).host();

    //inject api
    {
		DemoApi api;
		injectApi(context, "qcef.demoapi", &api);

    }
}

void QCefRenderProcessHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    releaeFrameHandler(frame);
}

bool QCefRenderProcessHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    (void)source_process;
    if (message->GetName() == "InvokeEvent")
    {
        invokeEvent(browser, message);
    }
    else if (message->GetName() == "InvokeCallback")
    {
        invokeCallback(browser, message);
    }
    else if (message->GetName() == "RunJavascript")
    {
        runJavaScript(browser, message);
    }
    else if(message->GetName() == "InvokeJsFunction")
    {
        invokeJsFunction(browser, message);
    }
    return true;
}
