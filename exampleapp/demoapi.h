#pragma once

class DemoApi : public QCefApiObject
{
    Q_OBJECT;

public:
    explicit DemoApi(QObject* parent = nullptr);
    ~DemoApi();


	Q_INVOKABLE void apiJson(QJsonDocument jsonDoc, JsFunctionWrapper callback);
};