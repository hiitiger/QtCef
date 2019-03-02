#include "stable.h"

#include "demoapi.h"

DemoApi::DemoApi(QObject* parent)
	: QObject(parent)
{

}

DemoApi::~DemoApi()
{

}

void DemoApi::apiJson(QJsonDocument jsonDoc, JsFunctionWrapper callback)
{
}


QFuture<QJsonDocument> DemoApi::apiJsonPromise(QJsonDocument jsonDoc)
{
    QFutureInterface<QJsonDocument> retfuture;
    return retfuture.future();
}

