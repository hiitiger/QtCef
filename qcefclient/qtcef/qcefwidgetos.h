#pragma once
#include "client_handler.h"
#include "qcefosrenderhandler.h"
#include "qcefpage.h"

//an example 
//you can implement your own CefWidget for use in Quick or Widget

class Q_DLL_QCEFCLIENT_API QCefOSWidget : public QWidget
{
    Q_OBJECT;

    Q_PROPERTY(QString url READ url WRITE setUrl)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString currentAddress READ currentAddress NOTIFY addressChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY enabledOperationsChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY enabledOperationsChanged)
    Q_PROPERTY(bool canRefresh READ canRefresh NOTIFY enabledOperationsChanged)
    Q_PROPERTY(bool canStop READ canStop NOTIFY enabledOperationsChanged)

    Q_ENUMS(Status)

public:

    enum Status
    {
        None,
        Initing,
        Loading,
        Loadend,
        Error
    };

    explicit QCefOSWidget(QWidget* parent = NULL);
    ~QCefOSWidget();

    void setUrl(const QString& url);
    QString url() const;

    QString currentAddress();
    QString title();

    Status status();

    bool canGoBack();
    bool canGoForward();
    bool canRefresh();
    bool canStop();

signals:
    void statusChanged();
    void addressChanged(const QString& url);
    void titleChanged(const QString& title);
    void enabledOperationsChanged();

    void jsInvokeMsg(const QString& object, const QString& method, const QVariantList& args, const QString& callbackId);

public slots:
    void refresh();
    void goBack();
    void goForward();
    void stop();
    void updateView();
    void updateCursor(QCursor);
    void runJavascript(const QString& code);

    void disposeBrowser();
    void sendProcessMessage(CefProcessId, CefRefPtr<CefProcessMessage>);
    void trySendProcessMessages();

protected:
    virtual void resizeEvent(QResizeEvent*);
    virtual void closeEvent(QCloseEvent*);
    virtual void showEvent(QShowEvent*);
    virtual void hideEvent(QHideEvent *);
    virtual void customEvent(QEvent *);

    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void timerEvent(QTimerEvent *);

    virtual void paintEvent(QPaintEvent *);

    virtual bool nativeEvent(const QByteArray& type, void* message, long* result);

protected slots:
    virtual void onBrowserCreate();
    virtual void onOpenPopupRequest(const QString& url);
    virtual void onAddressChange(const QString& url);
    virtual void onTitleChange(const QString& title);
    virtual void onLoadingChanged(bool);
    virtual void onJsInvoke(const QString& object, const QString& method, const QVariantList& argList, const QString& callbackId);
    virtual void showDevTool();
    virtual void closeDevTool();

private slots:
    bool createBrowser();
    void layoutBrowser();
    void setBrowserVisible(bool visible);
    void loadUrl();

private:
    CefRefPtr<CefBrowser> getBrowser();
    CefRefPtr<QCefClientHandler> getClientHandler();

private:
    QString m_url;
    QString m_currentAddress;

    CefRefPtr<QCefPage> m_cefPage;
    CefRefPtr<QCefOSRenderHandler> m_osrHandler;

    CefRefPtr<QCefClientHandler> m_clientHandler;
    CefRefPtr<CefBrowser> m_browser;

    QBasicTimer m_trippleClickTimer;
    QPoint m_trippleClickPoint;

    QList<CefProcessMessageWrapper> m_pendingProcessMessages;

    Status m_status;

};