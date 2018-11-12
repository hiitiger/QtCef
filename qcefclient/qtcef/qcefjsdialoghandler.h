#pragma once

#include "include/cef_jsdialog_handler.h"


class QCefJSDialogHandler : public QObject, public CefJSDialogHandler
{
    Q_OBJECT;
    Q_ENUMS(QtJSDialogType)
public:

    enum QtJSDialogType{
        JSDIALOGTYPE_ALERT = 0,
        JSDIALOGTYPE_CONFIRM,
        JSDIALOGTYPE_PROMPT,
    } ;

    QCefJSDialogHandler(base::WeakPtr<QCefClientHandler> client);
    ~QCefJSDialogHandler();

protected:
    virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
        const CefString& origin_url,
        JSDialogType dialog_type,
        const CefString& message_text,
        const CefString& default_prompt_text,
        CefRefPtr<CefJSDialogCallback> callback,
        bool& suppress_message) OVERRIDE;

    virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
        const CefString& message_text,
        bool is_reload,
        CefRefPtr<CefJSDialogCallback> callback) OVERRIDE;

    virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) OVERRIDE{}

    virtual void OnDialogClosed(CefRefPtr<CefBrowser> browser) OVERRIDE {}

public slots:

    void showJsDialog(QtJSDialogType dialogType, QString text, QString defaultPromptText, QCefJSDialogCallbackWrapper callback);
    void showBeforeUnloadDialog(QCefBrowserWrapper browser,  QString text, QCefJSDialogCallbackWrapper callback);

    void onMessageBoxButtonClicked(QAbstractButton*);

private:
    IMPLEMENT_REFCOUNTING(QCefJSDialogHandler);
    base::WeakPtr<QCefClientHandler> m_client;
};
