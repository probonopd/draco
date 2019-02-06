TARGET = DracoDesktop
VERSION = 1.0.0
TEMPLATE = lib

QT += core network concurrent x11extras svg

HEADERS += \
    DesktopSettings.h \
    LDesktopUtils.h \
    LIconCache.h \
    LInputDevice.h \
    LuminaOS.h \
    LUtils.h \
    LuminaRandR.h \
    LuminaSingleApplication.h \
    LuminaThemes.h \
    LuminaX11.h \
    LuminaXDG.h \
    LFileInfo.h \
    ResizeMenu.h \
    XDGMime.h

SOURCES += \
    DesktopSettings.cpp \
    LDesktopUtils.cpp \
    LIconCache.cpp \
    LInputDevice.cpp \
    LuminaOS-template.cpp \
    LUtils.cpp \
    LuminaRandR-X11.cpp \
    LuminaSingleApplication.cpp \
    LuminaThemes.cpp \
    LuminaX11.cpp \
    LuminaXDG.cpp \
    LFileInfo.cpp \
    ResizeMenu.cpp \
    XDGMime.cpp

LIBS += -lc -lxcb -lxcb-xinput
LIBS += -lxcb -lxcb-randr
LIBS += -lc -lxcb -lxcb-ewmh -lxcb-icccm -lxcb-image -lxcb-composite -lxcb-damage -lxcb-util -lXdamage
