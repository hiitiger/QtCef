#pragma once
#include "jsfunctiondescriptor.h"


class QCefClientHandler;

class Q_DLL_QCEFCLIENT_API JsFunctionWrapper : public JsFunctionDescriptor
{
public:
    JsFunctionWrapper(){ ; }

    JsFunctionWrapper(QString guid, CefRefPtr<QCefClientHandler> client);

    virtual void invoke(QVariantList argument);

    static CefRefPtr<CefListValue> convertToCefList(QVariantList argument);

private:
    CefRefPtr<QCefClientHandler> m_client;
};

Q_DECLARE_METATYPE(JsFunctionWrapper)