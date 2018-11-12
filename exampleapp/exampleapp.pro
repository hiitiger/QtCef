TEMPLATE = app

QT += core
QT += gui
QT += widgets

include(../common/config.pri)

PRECOMPILED_HEADER = stable.h
INCLUDEPATH +=  ../common/src/cef
LIBS += qcefclient.lib

HEADERS += $$files(./demoapi.h)
SOURCES += $$files(./demoapi.cpp)


SOURCES += $$files(./main.cpp)


