TEMPLATE = lib

QT += core
QT += gui

include(../common/config.pri)

PRECOMPILED_HEADER = stable.h

INCLUDEPATH +=  ../common/src/cef

DEFINES += Q_DLL_QCEFCLIENT

LIBS += libcef_dll_wrapper.lib
LIBS += libcef.lib
LIBS += qjson-backport.lib

HEADERS += $$files(./*.h)
SOURCES += $$files(./*.cpp)

HEADERS += $$files(qtcef/*.h)
SOURCES += $$files(qtcef/*.cpp)

HEADERS += $$files(qtapiadapter/*.h)
SOURCES += $$files(qtapiadapter/*.cpp)


HEADERS += qthack/genericmetacall.h
SOURCES += qthack/genericmetacall.cpp

SOURCES += qthack/genericsignalmap.cpp
SOURCES += qthack/moc_genericsignalmap.cpp