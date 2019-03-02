#include "stable.h"
#include <Windows.h>
#include <iostream>
#include "demoapi.h"


DemoApi::DemoApi(QObject* parent)
    : QCefApiObject(parent)
{

}

DemoApi::~DemoApi()
{

}

void DemoApi::apiJson(QJsonDocument jsonDoc, JsFunctionWrapper callback)
{
	QVariantMap result;
	result.insert("raw", jsonDoc.toVariant());

	QVariantList arg;
	arg.push_back(QVariant::fromValue(QJsonDocument::fromVariant(result)));
	callback.invoke(arg);
}

QFuture<QJsonDocument> DemoApi::apiJsonPromise(QJsonDocument jsonDoc)
{
    QFutureInterface<QJsonDocument> retfuture;

    QTimer::singleShot(1000, [retfuture, jsonDoc]() mutable {

        QVariantMap result;
        result.insert("raw", jsonDoc.toVariant());

        retfuture.reportResult(QJsonDocument::fromVariant(result));
        retfuture.reportFinished();
    });

    return retfuture.future();
}

