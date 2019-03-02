#pragma once

namespace Qt
{

    struct Q_DLL_QCEFCLIENT_API  MetaCallWrapper
    {
        bool ok = false;
        QObject* object;
        QVariant res;
        QMetaMethod metaMethod;
        QVariantList args;

        bool MetaCallWrapper::run();
    };
}
