#include "stable.h"
#include "genericmetacall.h"

namespace Qt
{

    bool MetaCallWrapper::run()
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


        if (metaMethod.returnType() == QMetaType::Void)
        {
            ok = metaMethod.invoke(
                object,
                Qt::DirectConnection,
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
        }
        else
        {

            res = QVariant((QVariant::Type)metaMethod.returnType(), static_cast<void*>(nullptr));

            QGenericReturnArgument returnArgument(
                res.typeName(),
                const_cast<void*>(res.constData())
            );

            ok = metaMethod.invoke(
                object,
                Qt::DirectConnection,
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
        }

        return ok;
    }
};
