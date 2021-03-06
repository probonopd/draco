include($${top_srcdir}/src/src.pri)

QT += widgets concurrent

TARGET = $${DESKTOP_TARGET}-launcher
TEMPLATE = app

SOURCES += main.cpp dialog.cpp
HEADERS += dialog.h

INCLUDEPATH += $${top_srcdir}/src/lib/fm/libfm
SOURCES += \
    $${top_srcdir}/src/lib/fm/libfm/desktopfile.cpp \
    $${top_srcdir}/src/lib/fm/libfm/mimeutils.cpp \
    $${top_srcdir}/src/lib/fm/libfm/common.cpp \
    $${top_srcdir}/src/lib/fm/libfm/properties.cpp \
    $${top_srcdir}/src/lib/fm/libfm/fileutils.cpp
HEADERS += \
    $${top_srcdir}/src/lib/fm/libfm/desktopfile.h \
    $${top_srcdir}/src/lib/fm/libfm/mimeutils.h \
    $${top_srcdir}/src/lib/fm/libfm/common.h \
    $${top_srcdir}/src/lib/fm/libfm/properties.h \
    $${top_srcdir}/src/lib/fm/libfm/fileutils.h

INCLUDEPATH += $${top_srcdir}/src/lib
LIBS += -L$${top_builddir}/lib$${LIBSUFFIX} -l$${DESKTOP_TARGET_NAME}

DESTDIR = $${top_builddir}/bin
OBJECTS_DIR = $${DESTDIR}/.obj_launcher
MOC_DIR = $${DESTDIR}/.moc_launcher
RCC_DIR = $${DESTDIR}/.qrc_launcher

target.path = $${PREFIX}/bin
INSTALLS += target
