include($${top_srcdir}/src/src.pri)

QT += network widgets x11extras concurrent dbus

TARGET = $${DESKTOP_TARGET}-desktop
TEMPLATE = app

DESTDIR = $${top_builddir}/bin
OBJECTS_DIR = $${DESTDIR}/.obj_desktop
MOC_DIR = $${DESTDIR}/.moc_desktop
RCC_DIR = $${DESTDIR}/.qrc_desktop

INCLUDEPATH += $${top_srcdir}/src/lib $${top_srcdir}/src/power/lib
LIBS += -L$${top_builddir}/lib$${LIBSUFFIX} -l$${DESKTOP_TARGET_NAME}Desktop
CONFIG(staticlib) : RESOURCES += $${top_srcdir}/share/share.qrc

PKGCONFIG += xcb-atom xrandr x11

CONFIG(staticlib) {
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
}

SOURCES += \
    main.cpp \
    LXcbEventFilter.cpp \
    LSession.cpp \
    LDesktop.cpp \
    LDesktopBackground.cpp \
    LDesktopPluginSpace.cpp \
    LPanel.cpp \
    LWinInfo.cpp \
    AppMenu.cpp \
    SystemWindow.cpp \
    desktop-plugins/LDPlugin.cpp \
    $${top_srcdir}/src/power/lib/hotplug.cpp \
    $${top_srcdir}/src/power/lib/screens.cpp \
    $${top_srcdir}/src/power/lib/device.cpp \
    $${top_srcdir}/src/power/lib/powerkit.cpp

HEADERS += \
    LXcbEventFilter.h \
    LSession.h \
    LDesktop.h \
    LDesktopBackground.h \
    LDesktopPluginSpace.h \
    LPanel.h \
    LWinInfo.h \
    AppMenu.h \
    SystemWindow.h \
    panel-plugins/LPPlugin.h \
    panel-plugins/NewPP.h \
    panel-plugins/LTBWidget.h \
    desktop-plugins/LDPlugin.h \
    desktop-plugins/NewDP.h \
    $${top_srcdir}/src/power/lib/hotplug.h \
    $${top_srcdir}/src/power/lib/screens.h \
    $${top_srcdir}/src/power/lib/device.h \
    $${top_srcdir}/src/power/lib/powerkit.h

FORMS += SystemWindow.ui

include(panel-plugins/panel-plugins.pri)
include(desktop-plugins/desktop-plugins.pri)


# qtcopydialog
INCLUDEPATH += $${top_srcdir}/src/fm/libfm/qtcopydialog
SOURCES += $${top_srcdir}/src/fm/libfm/qtcopydialog/qtcopydialog.cpp \
           $${top_srcdir}/src/fm/libfm/qtcopydialog/qtfilecopier.cpp
HEADERS += $${top_srcdir}/src/fm/libfm/qtcopydialog/qtcopydialog.h \
           $${top_srcdir}/src/fm/libfm/qtcopydialog/qtfilecopier.h
FORMS   += $${top_srcdir}/src/fm/libfm/qtcopydialog/qtcopydialog.ui \
           $${top_srcdir}/src/fm/libfm/qtcopydialog/qtoverwritedialog.ui \
           $${top_srcdir}/src/fm/libfm/qtcopydialog/qtotherdialog.ui

# qthotkey
INCLUDEPATH += $${top_srcdir}/src/hotkey/QHotkey
SOURCES += \
    $${top_srcdir}/src/hotkey/QHotkey/qhotkey.cpp \
    $${top_srcdir}/src/hotkey/QHotkey/qhotkey_x11.cpp
HEADERS += \
    $${top_srcdir}/src/hotkey/QHotkey/qhotkey.h \
    $${top_srcdir}/src/hotkey/QHotkey/qhotkey_p.h


RESOURCES += desktop.qrc

target.path = $${PREFIX}/bin
INSTALLS += target

