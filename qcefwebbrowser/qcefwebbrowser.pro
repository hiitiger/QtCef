TEMPLATE = app

Qt += core

include(../common/config.pri)


PRECOMPILED_HEADER = stable.h
INCLUDEPATH +=  ../common/src/cef

LIBS += libcef_dll_wrapper.lib
LIBS += libcef.lib
LIBS += qjson-backport.lib

HEADERS += $$files(./*.h)
SOURCES += $$files(./*.cpp)

