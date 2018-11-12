#pragma once

class Q_DLL_QCEFCLIENT_API QCefClientApp : public QObject, public CefApp, public CefBrowserProcessHandler
{
    Q_OBJECT;
public:
    QCefClientApp();

    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }


    virtual void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line) OVERRIDE;

    virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar);

    virtual void OnContextInitialized() OVERRIDE;


signals:
    void contextInitlialized();

private:
    IMPLEMENT_REFCOUNTING(QCefClientApp);
};

