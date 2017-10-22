#include "stable.h"

#include "QCefOSRenderHandler.h"


QCefOSRenderHandler::QCefOSRenderHandler(HWND window)
    : m_window(window)
{
}

QCefOSRenderHandler::~QCefOSRenderHandler()
{

}

void QCefOSRenderHandler::setWindowRect(const QRect& rc)
{
    m_windowRect = rc;
}

CefOSDrawParam& QCefOSRenderHandler::lock_image()
{
    lock_.Acquire();
    return m_drawParam;
}

void QCefOSRenderHandler::release_image()
{
    lock_.Release();
}

bool QCefOSRenderHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    (void)browser; (void)rect;
    return false;
}

bool QCefOSRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    CEF_REQUIRE_UI_THREAD();

    rect.x = rect.y = 0;
    rect.width = DeviceToLogical(m_windowRect.width(),
        GetDeviceScaleFactor());
    rect.height = DeviceToLogical(m_windowRect.height(),
        GetDeviceScaleFactor());
    return true;
}

bool QCefOSRenderHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser, int viewX, int viewY, int& screenX, int& screenY)
{
    CEF_REQUIRE_UI_THREAD();

    if (!::IsWindow(m_window))
        return false;

    POINT screen_pt = {
        LogicalToDevice(viewX, GetDeviceScaleFactor()),
        LogicalToDevice(viewY, GetDeviceScaleFactor())
    };
    ClientToScreen(m_window, &screen_pt);
    screenX = screen_pt.x;
    screenY = screen_pt.y;
    return true;
}

bool QCefOSRenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    CEF_REQUIRE_UI_THREAD();

    if (!::IsWindow(m_window))
        return false;

    CefRect view_rect;
    GetViewRect(browser, view_rect);

    screen_info.device_scale_factor = GetDeviceScaleFactor();
    screen_info.rect = view_rect;
    screen_info.available_rect = view_rect;
    return true;
}

void QCefOSRenderHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    (void)browser; (void)show;
}

void QCefOSRenderHandler::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    (void)browser; (void)rect;
}

void QCefOSRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
{
    QRect boundingDirtyRect;
    (void) dirtyRects; (void) type;
    {
      //  PRINTTIMECOST(CEF_OS_ONPAINT);
        base::AutoLock lock_scope(lock_);
        if (m_drawParam.image.width() != width || m_drawParam.image.height() != height)
        {
            size_t sz = width * height * 4;
            m_imageBuffer.resize(sz);
            memcpy_s(m_imageBuffer.data(), sz, buffer, sz);
            QImage image((const uchar*)m_imageBuffer.data(), width, height, QImage::Format_ARGB32);
            m_drawParam.image = image;
            m_drawParam.dirtyRectList.clear();
            boundingDirtyRect = QRect(0, 0, width, height);
        }
        else
        {
            uchar* imageBuffer = m_imageBuffer.data();
            m_drawParam.dirtyRectList.clear();
            for (RectList::const_iterator iter = dirtyRects.begin(); iter != dirtyRects.end(); ++iter)
            {
                CefRect dirtyRect = *iter;
                m_drawParam.dirtyRectList.append(QRect(dirtyRect.x, dirtyRect.y, dirtyRect.width, dirtyRect.height));
                int stopRow = dirtyRect.y + dirtyRect.height;
                for (int row = dirtyRect.y; row < stopRow; row++)
                {
                    int w = dirtyRect.width;
                    int offset = (width*row + dirtyRect.x) * 4;
                    unsigned char* pDest = (unsigned char*)imageBuffer + offset;
                    const unsigned char* pSrc = (const unsigned char*)buffer + offset;
                    memcpy(pDest, pSrc, w * 4);
                }
            }
            QRegion clipRegion;
            clipRegion.setRects(m_drawParam.dirtyRectList.constData(), m_drawParam.dirtyRectList.size());

            boundingDirtyRect = clipRegion.boundingRect();
        }
    }
    emit viewUpdated(boundingDirtyRect);
}

void QCefOSRenderHandler::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, CursorType type, const CefCursorInfo& custom_cursor_info)
{
    (void) custom_cursor_info; (void) cursor;

    static QMap<CursorType, Qt::CursorShape> shapeMap;
    if (shapeMap.isEmpty())
    {
        shapeMap.insert(CT_POINTER, Qt::ArrowCursor);
        shapeMap.insert(CT_HAND,    Qt::PointingHandCursor);
        shapeMap.insert(CT_IBEAM,   Qt::IBeamCursor);
        shapeMap.insert(CT_CROSS,   Qt::CrossCursor);
        shapeMap.insert(CT_HELP,    Qt::WhatsThisCursor);	
    }
    Qt::CursorShape shape = Qt::ArrowCursor;
    if (shapeMap.find(type) != shapeMap.end())
    {
        shape = shapeMap[type];
    }
    emit cursorUpdated(shape);
}

static bool g_enableCefScale = true;

Q_DLL_QCEFCLIENT_API void enableCefScale(bool b)
{
    g_enableCefScale = b;
}

Q_DLL_QCEFCLIENT_API bool isEnableCefScale()
{
    return g_enableCefScale;
}

Q_DLL_QCEFCLIENT_API float GetDeviceScaleFactor()
{
    static float scale_factor = 1.0;
    static bool initialized = false;
    if (isEnableCefScale())
    {
        if (!initialized)
        {
            HDC screen_dc = ::GetDC(NULL);
            int dpi_x = GetDeviceCaps(screen_dc, LOGPIXELSX);
            scale_factor = static_cast<float>(dpi_x) / 96.0f;
            ::ReleaseDC(NULL, screen_dc);
            initialized = true;
        }
    }
    return scale_factor;
}
