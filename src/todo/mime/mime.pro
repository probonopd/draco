include($${top_srcdir}/src/src.pri)

QT += widgets

TARGET = $${DESKTOP_TARGET}-settings-mime
TEMPLATE = app

SOURCES += main.cpp settingsdialog.cpp
HEADERS += settingsdialog.h

INCLUDEPATH += $${top_srcdir}/src/fm/libfm
SOURCES += \
    $${top_srcdir}/src/fm/libfm/icondlg.cpp \
    $${top_srcdir}/src/fm/libfm/fileutils.cpp \
    $${top_srcdir}/src/fm/libfm/applicationdialog.cpp \
    $${top_srcdir}/src/fm/libfm/properties.cpp \
    $${top_srcdir}/src/fm/libfm/common.cpp \
    $${top_srcdir}/src/fm/libfm/mimeutils.cpp \
    $${top_srcdir}/src/fm/libfm/desktopfile.cpp

HEADERS += \
    $${top_srcdir}/src/fm/libfm/icondlg.h \
    $${top_srcdir}/src/fm/libfm/fileutils.h \
    $${top_srcdir}/src/fm/libfm/applicationdialog.h \
    $${top_srcdir}/src/fm/libfm/properties.h \
    $${top_srcdir}/src/fm/libfm/common.h \
    $${top_srcdir}/src/fm/libfm/mimeutils.h \
    $${top_srcdir}/src/fm/libfm/desktopfile.h

DESTDIR = $${top_builddir}/bin
OBJECTS_DIR = $${DESTDIR}/.obj_settings-mime
MOC_DIR = $${DESTDIR}/.moc_settings-mime
RCC_DIR = $${DESTDIR}/.qrc_settings-mime

target.path = $${PREFIX}/bin
desktop.files += $${TARGET}.desktop
desktop.path += $${PREFIX}/share/applications
INSTALLS += target desktop
