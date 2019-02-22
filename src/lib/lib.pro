include($${top_srcdir}/src/src.pri)

QT += widgets network concurrent x11extras
TARGET = $${DESKTOP_TARGET_NAME}Desktop
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
    LuminaThemes.h \
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
    LuminaThemes.cpp \
    LuminaX11.cpp \
    LuminaXDG.cpp \
    LFileInfo.cpp \
    ResizeMenu.cpp \
    XDGMime.cpp

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

target.path = $${LIBDIR}
docs.path = $${DOCDIR}/$${DESKTOP_TARGET}-desktop-$${VERSION}
docs.files += \
    $${top_srcdir}/docs/LICENSE-lumina \
    $${top_srcdir}/docs/LICENSE-powerkit \
    $${top_srcdir}/docs/README.md
includes.path = $${PREFIX}/include/$${DESKTOP_TARGET_NAME}Desktop
includes.files = $${HEADERS}

!CONFIG(staticlib) {
    CONFIG += create_prl no_install_prl create_pc
    QMAKE_PKGCONFIG_NAME = $${DESKTOP_TARGET_NAME}Desktop
    QMAKE_PKGCONFIG_DESCRIPTION = $${DESKTOP_TARGET_NAME} Desktop Library
    QMAKE_PKGCONFIG_LIBDIR = $$target.path
    QMAKE_PKGCONFIG_INCDIR = $$includes.path
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig
    INSTALLS += target includes
}

INSTALLS += docs
