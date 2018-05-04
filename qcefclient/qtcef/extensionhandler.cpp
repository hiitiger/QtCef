#include "stable.h"
#include "extensionhandler.h"
#include "qcefclient.h"

QClientFileResourceHandler::QClientFileResourceHandler()
{
    file_range_invalid_ = false;
    file_range_start_ = 0;
    file_range_end_ = 0;
    file_size_ = 0;
    offset_ = 0;
}

bool QClientFileResourceHandler::ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();

    QString urlStr = QString::fromStdWString(request->GetURL().ToWString());
    QUrl url(urlStr);
    switch (request->GetResourceType())
    {
    case RT_STYLESHEET:
        m_mimeType = "text/css";
        break;
    case RT_SCRIPT:
        m_mimeType = "text/javascript";
        break;
    case RT_IMAGE:
        m_mimeType = "image";
        break;
    case RT_FONT_RESOURCE:
        m_mimeType = "font/opentype";
        break;
    case RT_XHR:
        m_mimeType = "text/plain";
        break;
    case RT_MEDIA:
        {
            QString path = url.path();
            if (path.endsWith(".mp4"))
            {
                m_mimeType = "video/mp4";
            }
            else
            {
                m_mimeType = "text/html";
            }
        }
        break;
    default:
        m_mimeType = "text/html";
        break;
    }


    CefRequest::HeaderMap headerMap;
    request->GetHeaderMap(headerMap);
    CefRequest::HeaderMap::iterator range = headerMap.find("Range");
    if (range != headerMap.end())
    {
        std::string range_str = range->second;
        if (range_str.find(",") != std::string::npos)
        {
            // Multiple range headers are not supported.
            // See http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html#sec19.2
            // for the necessary implementation details.
            file_range_invalid_ = true;
        }
        else
        {
            size_t pos = range_str.find("-");
            size_t start_pos = range_str.find("=");
            std::string range_start_str = range_str.substr(start_pos + 1, pos - start_pos - 1);
            file_range_start_ = atoi(range_start_str.c_str());
            std::string range_end_str =
                range_str.substr(pos + 1, std::string::npos);
            file_range_end_ = atoi(range_end_str.c_str());
        }

        QString resourcePath = QUrl::fromPercentEncoding(urlStr.toUtf8());
        resourcePath = resourcePath.replace("qcef-file://", "");
        resourcePath = resourcePath.left(1) + ":" + resourcePath.mid(1);

        if (QFile::exists(resourcePath))
        {
            m_file.reset(new QFile(resourcePath));
            m_file->open(QIODevice::ReadOnly);
            file_size_ = m_file->size();
            if (file_range_end_ == 0)
                file_range_end_ = file_size_;

            if (!file_range_invalid_ && (file_range_start_ >= file_range_end_ ||
                file_range_end_ > file_size_))
            {
                file_range_invalid_ = true;
            }

            if (!file_range_invalid_)
            {
                offset_ = file_range_start_;
                m_file->seek(offset_);
            }
            else
            {
                m_file->close();
                m_file.reset(nullptr);
            }
        }
    }

    else
    {
        QString resourcePath = QUrl::fromPercentEncoding(urlStr.toUtf8());
        resourcePath = resourcePath.replace("qcef-file://", "");
        resourcePath = resourcePath.left(1) + ":" + resourcePath.mid(1);
        if (QFile::exists(resourcePath))
        {
            m_file.reset(new QFile(resourcePath));
            m_file->open(QIODevice::ReadOnly);
        }
    }

    callback->Continue();

    return true;
}

void QClientFileResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl)
{
    CEF_REQUIRE_IO_THREAD();

    CefResponse::HeaderMap headerMap;
    response->GetHeaderMap(headerMap);
    headerMap.insert(std::make_pair("Cache-Control", "no-cache, no-store, must-revalidate"));
    headerMap.insert(std::make_pair("Access-Control-Allow-Origin", "*"));

    if (file_size_ > 0)
    {
        if (file_range_invalid_)
        {
            response->SetStatus(416);
            response->SetStatusText("Requested Range Not Satisfiable");
        }
        else
        {
            response->SetStatus(206);
            response->SetStatusText("Partial Content");
        }
        std::stringstream ss;
        ss << "0-" << file_size_;
        headerMap.insert(make_pair("Accept-Ranges", ss.str()));
        ss.str("");

        ss << "bytes " << file_range_start_ << "-" << file_range_end_ << "/" << file_size_;
        headerMap.insert(make_pair("Content-Range", ss.str()));
        ss.str("");
        response_length = file_range_end_ - file_range_start_;

        ss << response_length;
        headerMap.insert(make_pair("Content-Length", ss.str()));
        ss.str("");

        response->SetHeaderMap(headerMap);
    }
    else
    {
        response->SetHeaderMap(headerMap);
        response->SetMimeType(m_mimeType.toStdWString());
        response_length = m_file ? m_file->size() : 0;
        response->SetStatus(response_length ? 200 : 204);
    }
    redirectUrl = "";
}

bool QClientFileResourceHandler::ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback)
{
    CEF_REQUIRE_IO_THREAD();

    if (bytes_to_read == 0 || !m_file)
    {
        bytes_to_read = 0;
        return false;
    }

    if (file_size_ > 0 && !file_range_invalid_)
    {
        if (offset_ < file_range_end_)
        {
            int transfer_size = std::min(bytes_to_read, static_cast<int>(file_range_end_ - offset_));
            transfer_size = m_file->read((char*)data_out, transfer_size);
            offset_ += transfer_size;
            bytes_read = transfer_size;
        }
        else
        {
            m_file->close();
            m_file.reset(nullptr);
        }
    }
    else
    {
        bytes_read = m_file->read((char*)data_out, bytes_to_read);
        if (m_file->pos() >= m_file->size())
        {
            m_file->close();
            m_file.reset(nullptr);
        }
    }

    return true;
}
