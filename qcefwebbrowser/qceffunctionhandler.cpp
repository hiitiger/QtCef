#include "stable.h"
#include "qceffunctionhandler.h"

static QMap<int, QString> g_typeNames;


void setupTypeNames()
{
    qRegisterMetaType<JsFunctionWrapper>("JsFunctionWrapper");
    qRegisterMetaType<QJsonDocument>("QJsonDocument");
    qRegisterMetaType<QFuture<QJsonDocument>>("QFuture<QJsonDocument>");

    g_typeNames.insert(QMetaType::QString, "string");
    g_typeNames.insert(QMetaType::Int, "int");
    g_typeNames.insert(QMetaType::QReal, "double");
    g_typeNames.insert(QMetaType::Float, "float");
    g_typeNames.insert(QMetaType::Bool, "bool");
    g_typeNames.insert(QMetaType::QVariantList, "array");

    g_typeNames.insert(QMetaTypeId<QJsonDocument>::qt_metatype_id(), "json");
    g_typeNames.insert(QMetaTypeId<JsFunctionWrapper>::qt_metatype_id(), "function");
}

QString getTypeName(int type)
{
    if (g_typeNames.contains(type))
    {
        return g_typeNames.value(type);
    }
    else
    {
        return QMetaType::typeName(type);
    }
}