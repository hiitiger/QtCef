#pragma once

class QClientFileResourceHandler : public CefResourceHandler
{
public:
    QClientFileResourceHandler();

    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
        CefRefPtr<CefCallback> callback);

    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64& response_length,
        CefString& redirectUrl);

    virtual bool ReadResponse(void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefCallback> callback);

    virtual bool CanGetCookie(const CefCookie& cookie) { (void)cookie; return true; }

    virtual bool CanSetCookie(const CefCookie& cookie) { (void)cookie; return true; }

    virtual void Cancel() { ; }

private:
    IMPLEMENT_REFCOUNTING(QClientFileResourceHandler);

    std::unique_ptr<QFile> m_file;
    QString m_mimeType;
    bool file_range_invalid_;
    qint64 file_range_start_;
    qint64 file_range_end_;
    qint64 file_size_;
    qint64 offset_;
};


class QClientFileHandlerFactory : public CefSchemeHandlerFactory {
public:
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request)
        OVERRIDE {
        (void)browser;
        (void)frame;
        (void)scheme_name;
        (void)request;
        return new QClientFileResourceHandler();
    }
private:
    IMPLEMENT_REFCOUNTING(QClientFileHandlerFactory);
};