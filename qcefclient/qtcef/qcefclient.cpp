#include "stable.h"
#include "qcefclient.h"
#include "qcefjsdialoghandler.h"
#include "client_handler.h"

QCefClient::QCefClient(QObject* parent /*= nullptr*/)
    : QObject(parent)
{
    qRegisterMetaType<QCefJSDialogCallbackWrapper>("QCefJSDialogCallbackWrapper");
    qRegisterMetaType<QCefJSDialogHandler::QtJSDialogType>("QtJSDialogType");

    qRegisterMetaType<QCefBrowserWrapper>("QCefBrowserWrapper");
    qRegisterMetaType<QCefListValueWrapper>("QCefListValueWrapper");

    qRegisterMetaType<JsFunctionDescriptor>("JsFunctionDescriptor");
    qRegisterMetaType<JsFunctionWrapper>("JsFunctionWrapper");
    qRegisterMetaType<QJsonDocument>("QJsonDocument");

}

QCefClient::~QCefClient()
{

}


void QCefClient::initCefClient()
{
    m_mainArgs.reset(new CefMainArgs(GetModuleHandle(NULL)));
    m_cefapp = new QCefClientApp;
    connect(m_cefapp, SIGNAL(contextInitlialized()), this, SLOT(setupPlugin()));

    CefSettings settings;
    settings.no_sandbox = true;
    settings.single_process = false;
    settings.multi_threaded_message_loop = true;
    settings.windowless_rendering_enabled = true;
    settings.log_severity = LOGSEVERITY_ERROR;

    CefString(&settings.log_file) = logPath().append("\\").append("qcef.log").toUtf8().constData();
    CefString(&settings.browser_subprocess_path) = webHelperPath().toUtf8().constData();
    CefString(&settings.cache_path) = cachePath().toUtf8().constData();
    CefString(&settings.user_agent) = userAgentForWebBrowser().toUtf8().constData();

    settings.persist_session_cookies = true;
    CefInitialize(*m_mainArgs, settings, m_cefapp.get(), NULL);
}



void QCefClient::shutDown()
{
   // CefPostTask(TID_UI, base::Bind(CefShutdown));
    __try {
        CefShutdown();
    }
    __except (EXCEPTION_CONTINUE_EXECUTION){

    }
}

void QCefClient::setupPlugin()
{

}

QString QCefClient::appDir()
{
    return   qApp->applicationDirPath().replace("/", "\\");
}

QString QCefClient::webHelperPath()
{
    return   qApp->applicationDirPath().replace("/", "\\").append("\\").append("qcefwebbrowser.exe");
}

QString QCefClient::cachePath()
{
    QString cache = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    return cache.append("\\qcefcache");
}

QString QCefClient::logPath()
{
    QString log = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    log.append("\\log");
    return log;
}

QString QCefClient::pluginPath()
{
    QString plugin = qApp->applicationDirPath().replace("/", "\\").append("\\").append("plugins");
    return plugin;
}

QString QCefClient::userAgentForWebBrowser()
{
    //change this according to cef versoin
    return "";
}

CefBrowserSettings& QCefClient::defaultBrowseSetting()
{
    static CefBrowserSettings setting;
    setting.file_access_from_file_urls = STATE_DEFAULT;
    setting.universal_access_from_file_urls = STATE_DEFAULT;
    setting.web_security = STATE_ENABLED;
    return setting;
}

void QCefClient::setWebCookie(const QString& domain, const QString& url, const QString& name, const QString& value)
{
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
    CefCookie cookie;
    CefString(&cookie.name).FromWString(name.toStdWString());
    CefString(&cookie.value).FromWString(value.toStdWString());
    CefString(&cookie.domain).FromWString(domain.toStdWString());
    CefString(&cookie.path).FromWString(L"/");
    cookie.has_expires = true;
    cookie.expires.year = 2200;
    cookie.expires.month = 4;
    cookie.expires.day_of_week = 5;
    cookie.expires.day_of_month = 11;

    CefPostTask(TID_IO, NewCefRunnableMethod(manager.get(), &CefCookieManager::SetCookie, CefString(url.toUtf8()), cookie, nullptr));
}

void QCefClient::clearWebCookie(const QString& url, const QString& name)
{
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
    CefPostTask(TID_IO, NewCefRunnableMethod(manager.get(), &CefCookieManager::DeleteCookies, CefString(url.toUtf8()), CefString(name.toStdWString()), nullptr));
}

