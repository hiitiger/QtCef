#pragma once

class QClientApp : public CefApp
{
public:
    QClientApp();

    // CefApp methods:
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
        OVERRIDE{ return renderProcessHandler_; }

    virtual void OnRegisterCustomSchemes(
        CefRefPtr<CefSchemeRegistrar> registrar) OVERRIDE;

private:
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(QClientApp);

    CefRefPtr<QCefRenderProcessHandler> renderProcessHandler_;
};

