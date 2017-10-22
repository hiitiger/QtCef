#include "stable.h"
#include "genericsignalmap.h"

GenericSignalMapper::GenericSignalMapper(QMetaMethod mappedMethod, QObject* parent /*= nullptr*/)
    : QObject(parent)
    , m_method(mappedMethod)
{

}

GenericSignalMapper::~GenericSignalMapper()
{

}

void GenericSignalMapper::mapSlot()
{

}

void GenericSignalMapper::mappingSignal(void** _a)
{
    QVariantList args;
    int i = 0;
    foreach(QByteArray typeName, m_method.parameterTypes())
    {
        int type = QMetaType::type(typeName.constData());

        // preincrement: start with 1
        QVariant arg(type, _a[++i]);
        // (_a[0] is return value)
        args << arg;
    }
    emit mapped(sender(), m_method, args);
}
