#pragma once

/*
    1. cef widget may destroy  before ClientHandler, but CefPage live as long as than ClientHandler
    2. emit signal from cef UI thread to Qt UI thread slots 
*/
class QCefOSWidget;

class Q_DLL_QCEFCLIENT_API QCefPage : public QObject, public CefBaseRefCounted
{
    Q_OBJECT;
public:
    QCefPage() {}
    virtual ~QCefPage() {;}

    void setCefOSWidget(QCefOSWidget*);

    QPointer<QCefOSWidget> cefOSWidget();

signals:
    void browserCreate();
    void openPopupRequest(const QString& url);
    void addressChanged(const QString& url);
    void titleChanged(const QString& title);
    void loadingChanged(bool isLoading);
    void jsInvokeMsg(const QString& object, const QString& method, const QVariantList& argList, const QString& callbackId);
    void addEventListnerMsg(const QString& path);

    void requestShowDevTool();

public:
    virtual void onBrowserCreate();
    virtual bool openPopupUrl(const QString& url);

    virtual void onAddressChange(const QString& url);
    virtual void onTitleChange(const QString& title);

    virtual void onLoadingChanged(bool isLoading);

    virtual bool willBlockPlugin(const QString& pluginPath);

    virtual void onJsInvoke(const QString& object, const QString& method, const QVariantList& argList, const QString& callbackId);
    virtual void onAddEventListner(const QString& path);

    virtual void showDevTool();

private:
    IMPLEMENT_REFCOUNTING(QCefPage);

    QPointer<QCefOSWidget> m_cefOSWidget;
};