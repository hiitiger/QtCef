#pragma once

class QCefApiObject;
class GenericSignalMapper;

class Q_DLL_QCEFCLIENT_API QCefApiAdapter : public QObject
{
    Q_OBJECT;
public:
    QCefApiAdapter(QCefOSWidget* w, QObject* parent = nullptr);

    void initApi(QCefApiObject* apiImpl, QString parentPath, QString apiName);

    QString apiPath();

    void runJavascript(const QString& code);
    void invokeEvent(QString eventName, QVariantList arguments);

private slots:
    void onJsInvokeMsg(const QString& object, const QString& method, const QVariantList& args);
    void onAddEventListnerMsg(const QString& path);
    void onGenericSignal(QObject* sender, QMetaMethod signal, QVariantList args);

private:
    void parseSignal();

protected:
    QPointer<QCefOSWidget> m_cefWidget;
    QString m_appName;

    QObject* m_apiObject;
    QString m_apiName;
    QString m_parentPath;

    QList<GenericSignalMapper*> m_mappers;
};