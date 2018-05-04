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

