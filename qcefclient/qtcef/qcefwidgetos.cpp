#include "stable.h"
#include "cefeventtranslator.h"
#include "qcefwidgetos.h"

QCefOSWidget::QCefOSWidget(QWidget* parent /*= NULL*/)
    : QWidget(parent)
    , m_status(None)
{
    resize(800, 600);
    setMouseTracking(true);
}

QCefOSWidget::~QCefOSWidget()
{
    disposeBrowser();
}

void QCefOSWidget::setUrl(const QString& url)
{
    m_url = url;

    if (getBrowser().get())
    {
        loadUrl();
    }
    else
    {
        if (!url.isEmpty())
        {
            metaObject()->invokeMethod(this, "createBrowser", Qt::QueuedConnection);
        }
    }
}

QString QCefOSWidget::url() const
{
    return m_url;
}

QString QCefOSWidget::currentAddress()
{
    return m_currentAddress;
}

QString QCefOSWidget::title()
{
    return windowTitle();
}

QCefOSWidget::Status QCefOSWidget::status()
{
    return m_status;
}

bool QCefOSWidget::canGoBack()
{
    if (m_browser)
    {
        return m_browser->CanGoBack();
    }
    else
    {
        return false;
    }
}

bool QCefOSWidget::canGoForward()
{
    if (m_browser)
    {
        return m_browser->CanGoForward();
    }
    else
    {
        return false;
    }
}

bool QCefOSWidget::canRefresh()
{
    return(m_browser && m_status != Loading
        && !m_currentAddress.isEmpty());
}

bool QCefOSWidget::canStop()
{
    return m_browser && m_status == Loading;
}

void QCefOSWidget::refresh()
{
    if (m_status != QCefOSWidget::Loading)
    {
        m_clientHandler->Refresh();
    }
}

void QCefOSWidget::goBack()
{
    if (m_clientHandler)
    {
        m_clientHandler->Back();
    }
}

void QCefOSWidget::goForward()
{
    if (m_clientHandler)
    {
        m_clientHandler->Forward();
    }
}

void QCefOSWidget::stop()
{
    if (m_status == QCefOSWidget::Loading)
    {
        m_clientHandler->Stop();
    }
}

void QCefOSWidget::updateView()
{
    update();
}

void QCefOSWidget::updateCursor(QCursor c)
{
    setCursor(c);
}

void QCefOSWidget::runJavascript(const QString& code)
{
    CefRefPtr<CefProcessMessage> processMessage = CefProcessMessage::Create("RunJavascript");
    processMessage->GetArgumentList()->SetString(0, code.toStdWString());
    sendProcessMessage(PID_RENDERER, processMessage);
}

void QCefOSWidget::disposeBrowser()
{
    if (m_clientHandler.get() && !m_clientHandler->IsClosing())
    {
        _closeBrowser(m_clientHandler);
    }
    m_clientHandler = nullptr;
}

void QCefOSWidget::sendProcessMessage(CefProcessId pid, CefRefPtr<CefProcessMessage> processMessage)
{
    if (m_browser)
    {
        m_browser->SendProcessMessage(pid, processMessage);
        trySendProcessMessages();
    }
    else
    {
        CefProcessMessageWrapper message;
        message.pid = pid;
        message.processMessage = processMessage;
        m_pendingProcessMessages.append(m_pendingProcessMessages);
    }
}

void QCefOSWidget::trySendProcessMessages()
{
    if (m_browser)
    {
        Q_FOREACH(CefProcessMessageWrapper message, m_pendingProcessMessages)
        {
            m_browser->SendProcessMessage(message.pid, message.processMessage);
        }

        m_pendingProcessMessages.clear();
    }
}

void QCefOSWidget::resizeEvent(QResizeEvent* ev)
{
    __super::resizeEvent(ev);
    layoutBrowser();
}

void QCefOSWidget::closeEvent(QCloseEvent* ev)
{
    __super::closeEvent(ev);
}

void QCefOSWidget::showEvent(QShowEvent* ev)
{
    __super::showEvent(ev);
    setBrowserVisible(true);
}

void QCefOSWidget::hideEvent(QHideEvent * ev)
{
    __super::hideEvent(ev);
    setBrowserVisible(false);
}

void QCefOSWidget::customEvent(QEvent * ev)
{
    __super::customEvent(ev);
}

void QCefOSWidget::focusInEvent(QFocusEvent *event)
{
    __super::focusInEvent(event);

    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        browser->GetHost()->SendFocusEvent(true);
    }
}

void QCefOSWidget::focusOutEvent(QFocusEvent *event)
{
    __super::focusOutEvent(event);

    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        browser->GetHost()->SendFocusEvent(false);
    }
}

void QCefOSWidget::enterEvent(QEvent *event)
{
    __super::enterEvent(event);
}

void QCefOSWidget::leaveEvent(QEvent * event)
{
    __super::leaveEvent(event);

    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        QPoint pos =  this->mapFromGlobal(QCursor::pos());
        CefMouseEvent cefev;
        cefev.x = DeviceToLogical(pos.x(), GetDeviceScaleFactor());
        cefev.y = DeviceToLogical(pos.y(), GetDeviceScaleFactor());
        browser->GetHost()->SendMouseMoveEvent(cefev, true);
    }
}

void QCefOSWidget::mouseMoveEvent(QMouseEvent * event)
{
    __super::mouseMoveEvent(event);

    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        CefMouseEvent cefev;
        cefev.x = DeviceToLogical(event->pos().x(), GetDeviceScaleFactor());
        cefev.y = DeviceToLogical(event->pos().y(), GetDeviceScaleFactor());
        getKeyModifiers(cefev.modifiers, event->modifiers());
        getMouseModifiers(cefev.modifiers, event->buttons());
        browser->GetHost()->SendMouseMoveEvent(cefev, false);
    }
}

void QCefOSWidget::mousePressEvent(QMouseEvent * event)
{
    __super::mousePressEvent(event);

    event->accept();

    setFocus();

    CefRefPtr<CefBrowser> browser = getBrowser();

    if (browser.get())
    {
        CefMouseEvent cefev;
        cefev.x = DeviceToLogical(event->pos().x(), GetDeviceScaleFactor());
        cefev.y = DeviceToLogical(event->pos().y(), GetDeviceScaleFactor());
        getKeyModifiers(cefev.modifiers, event->modifiers());
        getMouseModifiers(cefev.modifiers, event->buttons());
        CefBrowserHost::MouseButtonType type = getMouseButtonType(event->button());

        int clickCount = 1;
        if (m_trippleClickTimer.isActive()
            && ((event->pos() - m_trippleClickPoint).manhattanLength() < QApplication::startDragDistance()))
        {
            clickCount = 3;
            m_trippleClickTimer.stop();
        }

        browser->GetHost()->SendMouseClickEvent(cefev, type, false, clickCount);
    }
}

void QCefOSWidget::mouseReleaseEvent(QMouseEvent * event)
{
    __super::mouseReleaseEvent(event);

    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        CefMouseEvent cefev;
        cefev.x = DeviceToLogical(event->pos().x(), GetDeviceScaleFactor());
        cefev.y = DeviceToLogical(event->pos().y(), GetDeviceScaleFactor());
        getKeyModifiers(cefev.modifiers, event->modifiers());
        getMouseModifiers(cefev.modifiers, event->buttons());
        CefBrowserHost::MouseButtonType type = getMouseButtonType(event->button());
        browser->GetHost()->SendMouseClickEvent(cefev, type, true, 1);
    }
}

void QCefOSWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    __super::mouseDoubleClickEvent(event);

    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        CefMouseEvent cefev;
        cefev.x = DeviceToLogical(event->pos().x(), GetDeviceScaleFactor());
        cefev.y = DeviceToLogical(event->pos().y(), GetDeviceScaleFactor());
        getKeyModifiers(cefev.modifiers, event->modifiers());
        getMouseModifiers(cefev.modifiers, event->buttons());
        CefBrowserHost::MouseButtonType type = getMouseButtonType(event->button());
        browser->GetHost()->SendMouseClickEvent(cefev, type, false, 2);

        m_trippleClickTimer.start(QApplication::doubleClickInterval(), this);
        m_trippleClickPoint = event->pos();
    }
}

void QCefOSWidget::wheelEvent(QWheelEvent * event)
{
    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        CefMouseEvent cefev;
        cefev.x = DeviceToLogical(event->pos().x(), GetDeviceScaleFactor());
        cefev.y = DeviceToLogical(event->pos().y(), GetDeviceScaleFactor());
        getKeyModifiers(cefev.modifiers, event->modifiers());
        getMouseModifiers(cefev.modifiers, event->buttons());
        int delta = event->delta();
        browser->GetHost()->SendMouseWheelEvent(cefev, isKeyDown(VK_SHIFT) ? delta : 0,
            !isKeyDown(VK_SHIFT) ? delta : 0);
    }
}

void QCefOSWidget::timerEvent(QTimerEvent *)
{

}

void QCefOSWidget::paintEvent(QPaintEvent *)
{
    if (m_osrHandler.get())
    {
        QPainter painter(this);
        CefOSDrawParam& image = m_osrHandler->lock_image();
        painter.drawImage(QPoint(), image.image);
        m_osrHandler->release_image();
    }
}

bool QCefOSWidget::winEvent(MSG * message, long * result)
{
    switch (message->message)
    {
    case WM_CLOSE:
        disposeBrowser();
        break;
    case WM_SYSCHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
    {
        if (QWidget::isActiveWindow() && hasFocus())
        {
            CefKeyEvent cefev;
            cefev.windows_key_code = message->wParam;
            cefev.native_key_code = message->lParam;
            cefev.is_system_key = message->message == WM_SYSCHAR ||
                message->message == WM_SYSKEYDOWN ||
                message->message == WM_SYSKEYUP;

            if (message->message == WM_KEYDOWN || message->message == WM_SYSKEYDOWN)
                cefev.type = KEYEVENT_RAWKEYDOWN;
            else if (message->message == WM_KEYUP || message->message == WM_SYSKEYUP)
                cefev.type = KEYEVENT_KEYUP;
            else
                cefev.type = KEYEVENT_CHAR;
            cefev.modifiers = getCefKeyboardModifiers(message->wParam, message->lParam);
            CefRefPtr<CefBrowser> browser = getBrowser();
            if (browser.get())
            {
                browser->GetHost()->SendKeyEvent(cefev);
            }
            *result = DefWindowProc(message->hwnd, message->message, message->wParam, message->lParam);
            return true;
        }
    }
    break;
    case WM_IME_STARTCOMPOSITION:
    case WM_IME_ENDCOMPOSITION:
    case WM_IME_COMPOSITION:
    case WM_IME_REQUEST:
    {
        if (isActiveWindow() && hasFocus())
        {
            *result = DefWindowProc(message->hwnd, message->message, message->wParam, message->lParam);

            return true;
        }
    }
    break;
    }
    return false;
}

bool QCefOSWidget::createBrowser()
{
    if (m_clientHandler.get())
    {
        return true;
    }

    CefWindowInfo info;
    info.SetAsWindowless(this->winId(), true);

    m_osrHandler = new  QCefOSRenderHandler(this->winId());
    connect(m_osrHandler, SIGNAL(viewUpdated(QRect)), SLOT(updateView()));
    connect(m_osrHandler, SIGNAL(cursorUpdated(QCursor)), SLOT(updateCursor(QCursor)));

    m_cefPage = new QCefPage();
    connect(m_cefPage, SIGNAL(browserCreate()), SLOT(onBrowserCreate()));
    connect(m_cefPage, SIGNAL(openPopupRequest(const QString&)), SLOT(onOpenPopupRequest(const QString&)));
    connect(m_cefPage, SIGNAL(addressChanged(const QString&)), SLOT(onAddressChange(const QString&)));
    connect(m_cefPage, SIGNAL(titleChanged(const QString&)), SLOT(onTitleChange(const QString&)));
    connect(m_cefPage, SIGNAL(loadingChanged(bool)), SLOT(onLoadingChanged(bool)));
    connect(m_cefPage, SIGNAL(jsInvokeMsg(const QString&, const QString&, const QVariantList&)), SLOT(onJsInvoke(const QString&, const QString&, const QVariantList&)));
    connect(m_cefPage, SIGNAL(requestShowDevTool()), SLOT(showDevTool()));

    m_clientHandler = new QCefClientHandler(m_cefPage, CefRefPtr<CefRenderHandler>(m_osrHandler.get()));

    CefBrowserSettings settings = QCefClient::defaultBrowseSetting();
    settings.windowless_frame_rate = 60;

    if (CefBrowserHost::CreateBrowser(info, m_clientHandler.get(), url().toStdWString(), settings, NULL))
    {
        m_status = QCefOSWidget::Initing;
        return true;
    }

     m_status = QCefOSWidget::Error;
    return false;
}

void QCefOSWidget::layoutBrowser()
{
    if (m_osrHandler.get())
    {
        m_osrHandler->setWindowRect(rect());
        CefRefPtr<CefBrowser> browser = getBrowser();
        if (browser.get())
        {
            browser->GetHost()->WasResized();
        }
    }
}

void QCefOSWidget::setBrowserVisible(bool visible)
{
    CefRefPtr<CefBrowser> browser = getBrowser();
    if (browser.get())
    {
        browser->GetHost()->WasHidden(!visible);
    }
}

void QCefOSWidget::loadUrl()
{
    m_clientHandler->LoadUrl(url());
}

CefRefPtr<CefBrowser> QCefOSWidget::getBrowser()
{
    CefRefPtr<CefBrowser> browser;
    if (m_clientHandler.get())
    {
        browser = m_clientHandler->GetBrowser();
    }
    return browser;
}

CefRefPtr<QCefClientHandler> QCefOSWidget::getClientHandler()
{
    return m_clientHandler;
}

void QCefOSWidget::onBrowserCreate()
{
    m_browser = getBrowser();

    setBrowserVisible(isVisible());
    layoutBrowser();
    loadUrl();

    trySendProcessMessages();
}

void QCefOSWidget::onOpenPopupRequest(const QString& url)
{
    QDesktopServices::openUrl(url);
}

void QCefOSWidget::onAddressChange(const QString& url)
{
    m_currentAddress = url;
    emit addressChanged(url);
    emit enabledOperationsChanged();
}

void QCefOSWidget::onTitleChange(const QString& title)
{
    setWindowTitle(title);
    emit titleChanged(title);
}

void QCefOSWidget::onLoadingChanged(bool loading)
{
    m_status = loading ? QCefOSWidget::Loading : QCefOSWidget::Loadend;
    emit statusChanged();
    emit enabledOperationsChanged();
}


void QCefOSWidget::onJsInvoke(const QString& object, const QString& method, const QVariantList& argList)
{
    (void)object; (void)method; (void)argList;
	emit jsInvokeMsg(object, method, argList);
}

void QCefOSWidget::showDevTool()
{
    if (!getBrowser())
    {
        return;
    }

    m_clientHandler->ShowDevTool(getBrowser());
}

void QCefOSWidget::closeDevTool()
{
    if (!getBrowser())
    {
        return;
    }

    getBrowser()->GetHost()->CloseDevTools();
}

