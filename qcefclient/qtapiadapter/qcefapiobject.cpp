#include"stable.h"
#include "qcefapiobject.h"

QCefApiObject::QCefApiObject(QObject* parent /*= nullptr*/)
    : QObject(parent)
    , m_adapter(nullptr)
{

}

QCefApiObject::~QCefApiObject()
{

}

void QCefApiObject::applyApiAdapter(QCefApiAdapter* adapter)
{
    m_adapter = adapter;
    apdaterApplayed();
}

QCefApiAdapter* QCefApiObject::adapter() const
{
    return m_adapter;
}

void QCefApiObject::apdaterApplayed()
{

}

