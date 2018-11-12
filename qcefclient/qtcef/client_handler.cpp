#include "stable.h"
#include <sstream>
#include <string>
#include "qcefpage.h"
#include "qcefjsdialoghandler.h"
#include "client_handler.h"

QCefClientHandler::QCefClientHandler(CefRefPtr<QCefPage> listner, CefRefPtr<CefRenderHandler> osrender)
    : clientListner_(listner)
    , osrenderHandler_(osrender)
    , is_closing_(0)
{
    jdDialogHandler_ = (CefJSDialogHandler*)new QCefJSDialogHandler(base::AsWeakPtr<QCefClientHandler>(this));
}

QCefClientHandler::~QCefClientHandler()
{
}


void QCefClientHandler::ResetClientListner()
{
    clientListner_ = NULL;
}


void QCefClientHandler::Refresh()
{
    CefRefPtr<CefBrowser> browser = GetBrowser();
    if (browser.get())
    {
        browser->Reload();
    }
}


void QCefClientHandler::Back()
{
    CefRefPtr<CefBrowser> browser = GetBrowser();
    if (browser.get() && browser->CanGoBack())
    {
        browser->GoBack();
    }
}


void QCefClientHandler::Forward()
{
    CefRefPtr<CefBrowser> browser = GetBrowser();
    if (browser.get() && browser->CanGoForward())
    {
        browser->GoForward();
    }
}

void QCefClientHandler::Stop()
{
    CefRefPtr<CefBrowser> browser = GetBrowser();
    if (browser.get())
    {
        browser->StopLoad();
    }
}

void QCefClientHandler::LoadUrl(const QString& url)
{
    base::AutoLock lock_scope(lock_);

    if (!url.isEmpty() && browser_.get())
    {
        CefString cefurl(url.toStdWString());
        if (cefurl == browser_->GetMainFrame()->GetURL())
        {
            //browser_->Reload();
        }
        else
        {
            browser_->StopLoad();
            browser_->GetMainFrame()->LoadURL(cefurl);
        }
    }
}

void QCefClientHandler::ExcuteJavascript(const QString& code)
{
    base::AutoLock lock_scope(lock_);
    if (browser_.get())
    {
        CefRefPtr<CefProcessMessage> processMessage = CefProcessMessage::Create("RunJavascript");
        processMessage->GetArgumentList()->SetString(0, code.toStdWString());
        browser_->SendProcessMessage(PID_RENDERER, processMessage);
    }
}

void QCefClientHandler::SendProcessMessage(const CefString& sigName, const CefString& args1)
{
    base::AutoLock lock_scope(lock_);
    if (browser_.get()) 
    {
        CefRefPtr<CefProcessMessage> processMessage = CefProcessMessage::Create(sigName);
        processMessage->GetArgumentList()->SetString(0, args1);
        browser_->SendProcessMessage(PID_RENDERER, processMessage);
    }
}

CefRefPtr<CefBrowser> QCefClientHandler::GetBrowser() const
{
    base::AutoLock lock_scope(lock_);
    return browser_;
}

int QCefClientHandler::GetBrowserId() const
{
    base::AutoLock lock_scope(lock_);
    return browser_id_;
}

void QCefClientHandler::OnBeforeContextMenu(CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefContextMenuParams > params, CefRefPtr< CefMenuModel > model)
{
    (browser); (frame); (params); (model); 
    CEF_REQUIRE_UI_THREAD();
    model->Clear();
}

bool QCefClientHandler::OnContextMenuCommand(CefRefPtr< CefBrowser > browser, CefRefPtr< CefFrame > frame, CefRefPtr< CefContextMenuParams > params, int command_id, CefContextMenuHandler::EventFlags event_flags)
{
    (browser); (frame); (params); (command_id); (event_flags);
    CEF_REQUIRE_UI_THREAD();
    return false;
}

void QCefClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
    CEF_REQUIRE_UI_THREAD();

    if (browser->IsSame(browser_) && frame->IsMain())
    {
        if (clientListner_.get())
        {
            clientListner_->onAddressChange(QString::fromStdWString(url.ToWString()));
        }
    }
}

void QCefClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();
    if (browser->IsSame(browser_))
    {
        if (clientListner_.get())
        {
            clientListner_->onTitleChange(QString::fromStdWString(title.ToWString()));
        }
    }
}

bool QCefClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, 
    CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{
    (browser); (frame); (target_frame_name); (target_disposition);(user_gesture); (popupFeatures); (windowInfo); (client); (settings); (no_javascript_access);
    CEF_REQUIRE_IO_THREAD();

    if (clientListner_.get())
    {
        clientListner_->openPopupUrl(QString::fromStdWString(target_url.ToWString()));
    }

    return true;
}

void QCefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (!GetBrowser())
    {
        {
            base::AutoLock lock_scope(lock_);
            browser_ = browser;
            browser_id_ = browser_->GetIdentifier();
        }

        if (clientListner_.get())
        {
            clientListner_->onBrowserCreate();
        }
    }
    //todo:
    else if (browser->IsPopup())
    {
        popup_browsers_.push_back(browser);

        auto cb = base::Bind(&CefBrowserHost::SetFocus);
        cb.Run(browser->GetHost().get(), true);

        CefPostTask(TID_UI, base::Bind(&CefBrowserHost::SetFocus, browser->GetHost().get(), true));
    }
}

bool QCefClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    if (GetBrowserId() == browser->GetIdentifier())
    {
        base::AutoLock lock_scope(lock_);
        InterlockedExchange(&is_closing_, 1);
    }

    return false;
}

void QCefClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();


    if (GetBrowserId() == browser->GetIdentifier())
    {
        base::AutoLock lock_scope(lock_);
        browser_ = NULL;
        ResetClientListner();
    }
    else if (browser->IsPopup())
    {
        BrowserList::iterator bit = popup_browsers_.begin();
        for (; bit != popup_browsers_.end(); ++bit)
        {
            if ((*bit)->IsSame(browser))
            {
                popup_browsers_.erase(bit);
                break;
            }
        }
    }
}

void QCefClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();
    (browser); (frame); (errorCode); (errorText); (failedUrl);
    //Cef LoadError happens before LoadStart and LoadEnd... just load a error page
    //frame->LoadString()
}

void QCefClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    if (browser->IsSame(browser_) && frame->IsMain())
    {
        if (clientListner_.get())
        {
            clientListner_->onLoadingChanged(true);
        }
    }
}

void QCefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int )
{
    if (browser->IsSame(browser_) && frame->IsMain())
    {
        if (clientListner_.get())
        {
            clientListner_->onLoadingChanged(false);
        }
    }
}

void QCefClientHandler::CloseAllBrowsers(bool force_close)
{
    BrowserList::const_iterator it = popup_browsers_.begin();
    for (; it != popup_browsers_.end(); ++it)
    {
        (*it)->GetHost()->CloseBrowser(force_close);
    }

    base::AutoLock lock_scope(lock_);
    if (browser_.get())
    {
        browser_->GetHost()->CloseBrowser(force_close);
    }
}


bool QCefClientHandler::IsClosing() 
{
    return InterlockedExchangeAdd(&is_closing_, 0) == 1;
}


void QCefClientHandler::ShowDevTool(CefRefPtr<CefBrowser> browser)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        CefPostTask(TID_UI, base::Bind(&QCefClientHandler::ShowDevTool, this, browser));
        return;
    }

    CefWindowInfo windowInfo;
    CefBrowserSettings settings;
    windowInfo.SetAsPopup(NULL, L"DevTool");
    browser->GetHost()->ShowDevTools(windowInfo, this, settings, CefPoint());
}

bool QCefClientHandler::OnDragEnter(CefRefPtr< CefBrowser > browser, CefRefPtr< CefDragData > dragData, CefDragHandler::DragOperationsMask mask)
{
    (void)browser; (void)dragData; (void)mask;
    return false;
}

bool QCefClientHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event, bool* is_keyboard_shortcut)
{
    (void)browser; (void)os_event; (void)is_keyboard_shortcut;
    switch (event.windows_key_code)
    {
    case VK_F11:
        {
        if (clientListner_)
        {
            ShowDevTool(browser);
        }
        return true;
        }
    default:
        break;
    }
    return false;
}


bool QCefClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
    (void)browser; (void)frame; (void)request; (void)is_redirect;
    return false;
}

void QCefClientHandler::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, TerminationStatus /*status*/)
{
    browser->Reload();
}

bool QCefClientHandler::OnBeforePluginLoad(CefRefPtr<CefBrowser> browser, const CefString& url, const CefString& policy_url, CefRefPtr<CefWebPluginInfo> info)
{
    (void)browser; (void)url; (void)policy_url;
    QString pluginPath = QString::fromStdWString(info->GetPath().ToWString());
    if (clientListner_.get() && clientListner_->willBlockPlugin(pluginPath))
    {
        return true;
    }
    return false;
}

CefRefPtr<CefResourceHandler> QCefClientHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
    return NULL;
}

bool QCefClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    (void)source_process;
    const std::string& message_name = message->GetName();
    if (message_name == "InvokeMethod") 
    {
        if (clientListner_.get())
        {
            CefRefPtr<CefListValue> arguments = message->GetArgumentList();
            if (arguments->GetSize() == 3)
            {
                QString objectName = QString::fromStdWString(arguments->GetString(0).ToWString());
                QString methodName = QString::fromStdWString(arguments->GetString(1).ToWString());
                CefRefPtr<CefListValue> cefListValue = arguments->GetList(2);

                QVariantList argList = convertArgList(this, cefListValue);

                clientListner_->onJsInvoke(objectName, methodName, argList);
            }         
        }
    }
    else if (message_name == "AddListner")
    {
        if (clientListner_.get())
        {
            CefRefPtr<CefListValue> arguments = message->GetArgumentList();
            if (arguments->GetSize() == 1)
            {
                QString eventPath = QString::fromStdWString(arguments->GetString(0).ToWString());
                clientListner_->onAddEventListner(eventPath);
            }
        }
    }
    
    return true;
}

bool QCefClientHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
    CefRequest::HeaderMap headerMap;
    request->GetHeaderMap(headerMap);
    headerMap.insert(std::make_pair("Accept-Language", "zh-CN"));
    request->SetHeaderMap(headerMap);

    return false;
}
