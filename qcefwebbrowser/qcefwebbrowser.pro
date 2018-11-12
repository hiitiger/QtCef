TEMPLATE = app

Qt += core

include(../common/config.pri)

PRECOMPILED_HEADER = stable.h

#INCLUDEPATH +=  ../common/src/cef

INCLUDEPATH +=  ../deps/cef3538

LIBS += libcef_dll_wrapper.lib
LIBS += libcef.lib

HEADERS += $$files(./ceffunc.h)
HEADERS += $$files(./client_app.h)
HEADERS += $$files(./demoapi.h)
HEADERS += $$files(./qcefeventfunctionhandler.h)
HEADERS += $$files(./qceffunctionhandler.h)
HEADERS += $$files(./qcefrenderprocesshandler.h)

SOURCES += $$files(./client_app.cpp)
SOURCES += $$files(./demoapi.cpp)
SOURCES += $$files(./qcefeventfunctionhandler.cpp)
SOURCES += $$files(./qceffunctionhandler.cpp)
SOURCES += $$files(./qcefrenderprocesshandler.cpp)


SOURCES += $$files(./main.cpp)

