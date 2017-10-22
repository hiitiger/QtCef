#pragma once

#include "client_handler.h"


Q_DLL_QCEFCLIENT_API void enableCefScale(bool);
Q_DLL_QCEFCLIENT_API bool isEnableCefScale();
Q_DLL_QCEFCLIENT_API float GetDeviceScaleFactor();

inline int LogicalToDevice(int value, float device_scale_factor) {
    float scaled_val = static_cast<float>(value) * device_scale_factor;
    return static_cast<int>(std::floor(scaled_val));
}
inline int DeviceToLogical(int value, float device_scale_factor) {
    float scaled_val = static_cast<float>(value) / device_scale_factor;
    return static_cast<int>(std::floor(scaled_val));
}

struct CefOSDrawParam
{
    QImage image;
    QVector<QRect> dirtyRectList;
};

class QCefOSRenderHandler : public QObject, public CefRenderHandler
{
    Q_OBJECT;
public:

    QCefOSRenderHandler(HWND window);
    ~QCefOSRenderHandler();

    void setWindowRect(const QRect& rc);
    CefOSDrawParam& lock_image();
    void release_image();

signals:
    void viewUpdated(QRect) ;
    void cursorUpdated(QCursor);

protected:

    virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser,
        CefRect& rect);

    virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect);

    virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
        int viewX,
        int viewY,
        int& screenX,
        int& screenY);

    virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
        CefScreenInfo& screen_info);
    
    virtual void OnPopupShow(CefRefPtr<CefBrowser> browser,
        bool show);

    virtual void OnPopupSize(CefRefPtr<CefBrowser> browser,
        const CefRect& rect);

    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer,
        int width, int height);

    virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
        CefCursorHandle cursor,
        CursorType type,
        const CefCursorInfo& custom_cursor_info);

private:
    IMPLEMENT_REFCOUNTING(QCefOSRenderHandler);

    QRect m_windowRect;
    HWND m_window;
    mutable base::Lock lock_;

    std::vector<uchar> m_imageBuffer;

    CefOSDrawParam m_drawParam;
};