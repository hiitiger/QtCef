
#pragma once

#include "client_app.h"
#include "client_handler.h"

class CefApp;
class CefBrowser;
class CefCommandLine;

class Q_DLL_QCEFCLIENT_API QCefClient : public QObject
{
    Q_OBJECT;
public:
    explicit QCefClient(QObject* parent = nullptr);
    ~QCefClient();

    void initCefClient();
    void shutDown();

    static QString appDir();
    static QString webHelperPath();
    static QString cachePath();
    static QString logPath();
    static QString pluginPath();
    
    static QString userAgentForWebBrowser();

    static CefBrowserSettings& defaultBrowseSetting();

    static void setWebCookie(const QString& domain, const QString& url, const QString& name, const QString& value);
    static void clearWebCookie(const QString& url,const QString& name);

public slots:
    void setupPlugin();

private:
    CefRefPtr<QCefClientApp> m_cefapp;
    std::unique_ptr<CefMainArgs> m_mainArgs;
};
