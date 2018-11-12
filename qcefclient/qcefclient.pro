TEMPLATE = lib

QT += core
QT += gui
QT += widgets

include(../common/config.pri)

PRECOMPILED_HEADER = stable.h

INCLUDEPATH +=  ../deps/cef3538

DEFINES += Q_DLL_QCEFCLIENT

LIBS += libcef_dll_wrapper.lib
LIBS += libcef.lib

HEADERS += $$files(qtcef/*.h)
SOURCES += $$files(qtcef/*.cpp)

HEADERS += $$files(qtapiadapter/*.h)
SOURCES += $$files(qtapiadapter/*.cpp)

HEADERS += qthack/genericmetacall.h
SOURCES += qthack/genericmetacall.cpp

#HEADERS += qthack/genericsignalmap.h
SOURCES += qthack/genericsignalmap.cpp
SOURCES += qthack/moc_genericsignalmap.cpp