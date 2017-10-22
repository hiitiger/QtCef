CharacterSet=1

INCLUDEPATH += .
INCLUDEPATH += ../common/include

CONFIG(debug, debug|release) {
    QMAKE_LIBDIR += ../bin/Debug
    QMAKE_LIBDIR += ../common/lib/debug

}
CONFIG(release, debug|release) {
    QMAKE_LIBDIR += ../bin/Release
    QMAKE_LIBDIR += ../common/lib/release
}

OBJECTS_DIR = $(SolutionDir)Objdir/$(ProjectName)/$(Configuration)

OUTPUTBIN_PATH = $(SolutionDir)bin/$(Configuration)

DESTDIR = $${OUTPUTBIN_PATH}
