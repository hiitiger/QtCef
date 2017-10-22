#pragma once

class QCefApiAdapter;
class Q_DLL_QCEFCLIENT_API QCefApiObject : public QObject
{
public:
    explicit QCefApiObject(QObject* parent = nullptr);
    ~QCefApiObject();

    void applyApiAdapter(QCefApiAdapter* adapter);
    QCefApiAdapter* adapter() const;

    virtual void apdaterApplayed(); 

private:

    QCefApiAdapter* m_adapter;
};