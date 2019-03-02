#pragma once

class DemoApi : public QObject
{
	Q_OBJECT;

public:
	explicit DemoApi(QObject* parent = nullptr);
	~DemoApi();


    Q_INVOKABLE void apiJson(QJsonDocument jsonDoc, JsFunctionWrapper callback);
    Q_INVOKABLE QFuture<QJsonDocument> apiJsonPromise(QJsonDocument jsonDoc);
};