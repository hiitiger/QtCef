#pragma once

class Q_DLL_QCEFCLIENT_API GenericSignalMapper : public QObject
{
    Q_OBJECT;
public:
    explicit GenericSignalMapper(QMetaMethod mappedMethod, QObject* parent = nullptr);
    ~GenericSignalMapper();

signals:
    void mapped(QObject *sender, QMetaMethod signal, QVariantList arguments);

private slots:
    void mapSlot();

private:
    void mappingSignal(void** _a);

private:

    QMetaMethod m_method;
};