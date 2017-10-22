TEMPLATE = app

QT += core
QT += gui

include(../common/config.pri)

PRECOMPILED_HEADER = stable.h
INCLUDEPATH +=  ../common/src/cef
LIBS += qcefclient.lib

HEADERS += $$files(./*.h)
SOURCES += $$files(./*.cpp)

