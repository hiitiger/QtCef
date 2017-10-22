#include "stable.h"

#include "QCefPage.h"



void QCefPage::setCefOSWidget(QCefOSWidget* w)
{
    m_cefOSWidget = w;
}

QPointer<QCefOSWidget> QCefPage::cefOSWidget()
{
    return m_cefOSWidget;
}

void QCefPage::onBrowserCreate()
{
    emit browserCreate();
}

bool QCefPage::openPopupUrl(const QString& url)
{
    emit openPopupRequest(url);
    return true;
}

void QCefPage::onAddressChange(const QString& url)
{
    emit addressChanged(url);
}

void QCefPage::onTitleChange(const QString& title)
{
    emit titleChanged(title);
}

void QCefPage::onLoadingChanged(bool isLoading)
{
    emit loadingChanged(isLoading);
}

bool QCefPage::willBlockPlugin(const QString& /*pluginPath*/)
{
    return false;
}

void QCefPage::onJsInvoke(const QString& object, const QString& method, const QVariantList& argList)
{
    emit jsInvokeMsg(object, method, argList);
}

void QCefPage::onAddEventListner(const QString& path)
{
    emit addEventListnerMsg(path);
}

void QCefPage::showDevTool()
{
    emit requestShowDevTool();
}
