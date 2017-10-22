#pragma once
#include "QCefPage.h"

class Q_DLL_QCEFCLIENT_API QCefClientHandler : public CefClient, public base::SupportsWeakPtr<QCefClientHandler>,
            public CefContextMenuHandler,
            public CefDisplayHandler,
            public CefLifeSpanHandler,
            public CefLoadHandler,
            public CefDragHandler,
            public CefKeyboardHandler,
            public CefRequestHandler


{
public:

    QCefClientHandler(CefRefPtr<QCefPage> listner, CefRefPtr<CefRenderHandler> osrender = NULL);
    ~QCefClientHandler();

    void ResetClientListner();

    CefRefPtr<QCefPage> GetCefPage() { return clientListner_; }

    void Refresh();
    void Back();
    void Forward();
    void Stop();
    void LoadUrl(const QString&);
  
    void ExcuteJavascript(const QString& code);
    void SendProcessMessage(const CefString& sigName, const CefString& args1);

    CefRefPtr<CefBrowser> GetBrowser() const;
    int GetBrowserId() const;

    void CloseAllBrowsers(bool force_close);

    bool IsClosing();

    void ShowDevTool(CefRefPtr<CefBrowser> browser);

    // CefClient methods:
    virtual CefRefPtr< CefContextMenuHandler > GetContextMenuHandler() OVERRIDE {return this;}
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE { return this; }
    virtual CefRefPtr< CefKeyboardHandler > GetKeyboardHandler() OVERRIDE { return this; }
    virtual CefRefPtr< CefRequestHandler > GetRequestHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE { return osrenderHandler_; }
    virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE { return jdDialogHandler_; }

    //CefContextMenuHandler
    virtual void OnBeforeContextMenu( CefRefPtr< CefBrowser > browser,
        CefRefPtr< CefFrame > frame, CefRefPtr< CefContextMenuParams > params, CefRefPtr< CefMenuModel > model ) OVERRIDE;
    virtual bool OnContextMenuCommand( CefRefPtr< CefBrowser > browser,
        CefRefPtr< CefFrame > frame, CefRefPtr< CefContextMenuParams > params, int command_id, CefContextMenuHandler::EventFlags event_flags ) OVERRIDE;

    // CefDisplayHandler methods:
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& url) OVERRIDE;

    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title) OVERRIDE;

    // CefLifeSpanHandler methods:
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& target_url,
        const CefString& target_frame_name,
        CefLifeSpanHandler::WindowOpenDisposition target_disposition,
        bool user_gesture,
        const CefPopupFeatures& popupFeatures,
        CefWindowInfo& windowInfo,
        CefRefPtr<CefClient>& client,
        CefBrowserSettings& settings,
        bool* no_javascript_access)  OVERRIDE;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // CefLoadHandler methods:
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl) OVERRIDE;

    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame) OVERRIDE;

    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode) OVERRIDE;

    //CefDragHandler
    virtual bool OnDragEnter( CefRefPtr< CefBrowser > browser,
        CefRefPtr< CefDragData > dragData, CefDragHandler::DragOperationsMask mask ) OVERRIDE;


    //CefKeyboardHandler
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
        const CefKeyEvent& event,
        CefEventHandle os_event,
        bool* is_keyboard_shortcut);

    //CefRequestHandler
    virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        bool is_redirect);
    virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
        TerminationStatus status) ;
    virtual bool OnBeforePluginLoad(CefRefPtr<CefBrowser> browser,
        const CefString& url,
        const CefString& policy_url,
        CefRefPtr<CefWebPluginInfo> info);
    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request);
    virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request);

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message);
private:
    CefRefPtr<QCefPage> clientListner_;
    CefRefPtr<CefRenderHandler> osrenderHandler_;
    CefRefPtr<CefJSDialogHandler> jdDialogHandler_;

    mutable base::Lock lock_;

    CefRefPtr<CefBrowser> browser_;
    int browser_id_;    
    typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
    BrowserList popup_browsers_;
    volatile unsigned long is_closing_;

    IMPLEMENT_REFCOUNTING(QCefClientHandler);
};



inline void _closeBrowser(CefRefPtr<QCefClientHandler>& clientHandler)
{
    __try{
        clientHandler->CloseAllBrowsers(true);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {

    }
}

