#-------------------------------------------------
#
# Project created by QtCreator 2012-10-09T15:43:08
#
#-------------------------------------------------

QT       -= gui

TARGET = qjson-backport
TEMPLATE = lib

DEFINES += Q_BUILD_JSONRPC

SOURCES += qjsonwriter.cpp \
    qjsonvalue.cpp \
    qjsonparser.cpp \
    qjsonobject.cpp \
    qjsondocument.cpp \
    qjsonarray.cpp \
    qjson.cpp

HEADERS += qjsonwriter_p.h \
    qjsonvalue.h \
    qjsonparser_p.h \
    qjson_p.h \
    qjsonobject.h \
    qjsonexport.h \
    qjsondocument.h \
    qjsonarray.h
