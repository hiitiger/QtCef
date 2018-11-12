#include "stable.h"
#include "genericmetacall.h"

namespace Qt
{

    QVariant  metaCall(QObject* object, QMetaMethod metaMethod, QVariantList args)
    {
        QList<QGenericArgument> arguments;

        for (int i = 0; i < args.size(); i++) {

            // Notice that we have to take a reference to the argument. A 
            // const_cast is needed because calling data() would detach 
            // the QVariant.

            QVariant& argument = args[i];

            QGenericArgument genericArgument(
                QMetaType::typeName(argument.userType()),
                const_cast<void*>(argument.constData())
                );

            arguments << genericArgument;
        }

        QVariant returnValue(QMetaType::type(metaMethod.typeName()),
            static_cast<void*>(NULL));

        QGenericReturnArgument returnArgument(
            metaMethod.typeName(),
            const_cast<void*>(returnValue.constData())
            );

        // Perform the call

        bool ok = metaMethod.invoke(
            object,
            Qt::AutoConnection, // In case the object is in another thread.
            returnArgument,
            arguments.value(0),
            arguments.value(1),
            arguments.value(2),
            arguments.value(3),
            arguments.value(4),
            arguments.value(5),
            arguments.value(6),
            arguments.value(7),
            arguments.value(8),
            arguments.value(9)
            );

        if (!ok) {
            qWarning() << "Calling" << metaMethod.methodSignature() << "failed.";
            return QVariant();
        }
        else {
            return returnValue;
        }
    }

};
