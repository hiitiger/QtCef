#include "stable.h"
#include "qcefjsdialoghandler.h"


QCefJSDialogHandler::QCefJSDialogHandler(base::WeakPtr<QCefClientHandler> client)
    : m_client(client)
{

}

QCefJSDialogHandler::~QCefJSDialogHandler()
{

}

bool QCefJSDialogHandler::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, const CefString& accept_lang, JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message)
{
    (void)origin_url;(void)accept_lang; (void)suppress_message;

    QString text = QString::fromStdWString(message_text.ToWString());
    QString promptText = QString::fromStdWString(default_prompt_text.ToWString());
    QtJSDialogType dialogType = static_cast<QtJSDialogType>(dialog_type);
    QCefJSDialogCallbackWrapper callbackWrapper;
    callbackWrapper.callback = callback;

    metaObject()->invokeMethod(this, "showJsDialog", Qt::QueuedConnection, Q_ARG(QtJSDialogType, dialogType), Q_ARG(QString, text), Q_ARG(QString, promptText), Q_ARG(QCefJSDialogCallbackWrapper, callbackWrapper));

    return true;
}

bool QCefJSDialogHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString& message_text, bool is_reload, CefRefPtr<CefJSDialogCallback> callback)
{
    (void)is_reload;

    QCefBrowserWrapper browserWrapper;
    browserWrapper.browser = browser;
    QString text = QString::fromStdWString(message_text.ToWString());
    QCefJSDialogCallbackWrapper callbackWrapper;
    callbackWrapper.callback = callback;

    metaObject()->invokeMethod(this, "showBeforeUnloadDialog", Qt::QueuedConnection, Q_ARG(QCefBrowserWrapper, browserWrapper), Q_ARG(QString, text), Q_ARG(QCefJSDialogCallbackWrapper, callbackWrapper));

    return true;
}

void QCefJSDialogHandler::showJsDialog(QtJSDialogType dialogType, QString text, QString defaultPromptText, QCefJSDialogCallbackWrapper callback)
{
    switch (dialogType)
    {
    case QCefJSDialogHandler::JSDIALOGTYPE_ALERT:
        {
            QMessageBox* msgBox = new QMessageBox();
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->setWindowTitle(tr("Alert"));
            msgBox->setText(text);
            msgBox->setModal(false);
            msgBox->open(this, SLOT(onMessageBoxButtonClicked(QAbstractButton*)));
            msgBox->setProperty("jsCallBackWrapper", QVariant::fromValue(callback));
        }
        break;
    case QCefJSDialogHandler::JSDIALOGTYPE_CONFIRM:
        {
            QMessageBox* msgBox = new QMessageBox();
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            msgBox->setWindowTitle(tr("Confirm"));
            msgBox->setText(text);
            msgBox->setModal(false);
            msgBox->open(this, SLOT(onMessageBoxButtonClicked(QAbstractButton*)));
            msgBox->setProperty("jsCallBackWrapper", QVariant::fromValue(callback));
        }
        break;
    case QCefJSDialogHandler::JSDIALOGTYPE_PROMPT:
    {
        //
    }
        break;
    default:
        break;
    }
}

void QCefJSDialogHandler::showBeforeUnloadDialog(QCefBrowserWrapper browser, QString text, QCefJSDialogCallbackWrapper callback)
{
    CefRefPtr<QCefPage> cefPage = m_client->GetCefPage();
    if (!cefPage.get())
    {
        return;
    }

    QCefOSWidget* cefWidget = cefPage->cefOSWidget();
    if (!cefWidget)
    {
        return;
    }

    QWidget* window = cefWidget->window();
    QMessageBox* msgBox = new QMessageBox(window);
    msgBox->setAttribute(Qt::WA_DeleteOnClose);
    msgBox->setStandardButtons(QMessageBox::Ok);
    msgBox->setWindowTitle(tr("Question"));
    msgBox->setText("Leave this page?");
    msgBox->setModal(true);
    msgBox->open(this, SLOT(onMessageBoxButtonClicked(QAbstractButton*)));
    msgBox->setProperty("jsCallBackWrapper", QVariant::fromValue(callback));
}

void QCefJSDialogHandler::onMessageBoxButtonClicked(QAbstractButton* button)
{
    QMessageBox* msgBox = qobject_cast<QMessageBox*>(sender());
    bool success = msgBox->buttonRole(button) == QMessageBox::Ok;
    QCefJSDialogCallbackWrapper callback = qvariant_cast<QCefJSDialogCallbackWrapper>(msgBox->property("jsCallBackWrapper"));
    callback.callback->Continue(success, CefString());
}

