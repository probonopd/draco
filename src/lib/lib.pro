
# common
include($${top_srcdir}/src/src.pri)

# qt
QT += widgets network concurrent x11extras

# lib
TARGET = $${DESKTOP_TARGET_NAME}
TEMPLATE = lib
DESTDIR = $${top_builddir}/lib$${LIBSUFFIX}
OBJECTS_DIR = $${DESTDIR}/.obj_libdesktop
MOC_DIR = $${DESTDIR}/.moc_libdesktop
RCC_DIR = $${DESTDIR}/.qrc_libdesktop

#INCLUDEPATH += $${top_srcdir}/src/power/lib

HEADERS += \
    DesktopSettings.h \
    LDesktopUtils.h \
    LIconCache.h \
    LInputDevice.h \
    LUtils.h \
    LuminaRandR.h \
    LuminaSingleApplication.h \
    LuminaX11.h \
    LuminaXDG.h \
    LFileInfo.h \
    ResizeMenu.h \
    XDGMime.h \
    common.h

SOURCES += \
    DesktopSettings.cpp \
    LDesktopUtils.cpp \
    LIconCache.cpp \
    LInputDevice.cpp \
    LUtils.cpp \
    LuminaRandR-X11.cpp \
    LuminaSingleApplication.cpp \
    LuminaX11.cpp \
    LuminaXDG.cpp \
    LFileInfo.cpp \
    ResizeMenu.cpp \
    XDGMime.cpp

# misc config files etc
RESOURCES += $${top_srcdir}/share/share.qrc

# qtcopydialog
INCLUDEPATH += fm/libfm/qtcopydialog
SOURCES += fm/libfm/qtcopydialog/qtcopydialog.cpp \
           fm/libfm/qtcopydialog/qtfilecopier.cpp
HEADERS += fm/libfm/qtcopydialog/qtcopydialog.h \
           fm/libfm/qtcopydialog/qtfilecopier.h
FORMS   += fm/libfm/qtcopydialog/qtcopydialog.ui \
           fm/libfm/qtcopydialog/qtoverwritedialog.ui \
           fm/libfm/qtcopydialog/qtotherdialog.ui

# qthotkey
INCLUDEPATH += hotkey/QHotkey
SOURCES += \
    hotkey/QHotkey/qhotkey.cpp \
    hotkey/QHotkey/qhotkey_x11.cpp
HEADERS += \
    hotkey/QHotkey/qhotkey.h \
    hotkey/QHotkey/qhotkey_p.h

# xcb
PKGCONFIG += \
    xcb \
    xcb-xinput \
    xcb-randr \
    xcb-ewmh \
    xcb-icccm \
    xcb-image \
    xcb-composite \
    xcb-damage \
    xcb-util \
    xdamage

# stuff we want to install
target.path = $${LIBDIR}
docs.path = $${DOCDIR}/$${DESKTOP_TARGET}-desktop-$${VERSION}
docs.files += $${top_srcdir}/docs/LICENSE $${top_srcdir}/docs/README.md
includes.path = $${PREFIX}/include/$${DESKTOP_TARGET_NAME}/Desktop
includes.files = $${HEADERS}

# pkg-config
CONFIG += create_prl no_install_prl create_pc
QMAKE_PKGCONFIG_NAME = $${DESKTOP_TARGET_NAME}
QMAKE_PKGCONFIG_DESCRIPTION = $${DESKTOP_TARGET_NAME} Library
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $${PREFIX}/include/$${DESKTOP_TARGET_NAME}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

# install
INSTALLS += target includes docs
