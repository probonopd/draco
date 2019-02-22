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
!CONFIG(staticlib): QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

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
    SettingsMenu.cpp \
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
    SettingsMenu.h \
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

#RESOURCES+= Lumina-DE.qrc

#desktop.path = $${L_SESSDIR}
#desktop.files = Lumina-DE.desktop

#icons.files = Lumina-DE.png

#fluxconf.files = fluxboxconf/fluxbox-init-rc \
#			fluxboxconf/fluxbox-keys
#fluxconf.path = $${L_SHAREDIR}/lumina-desktop/

#defaults.files = defaults/luminaDesktop.conf \
#		defaults/compton.conf \
#		audiofiles/Logout.ogg \
#		audiofiles/Login.ogg \
#		audiofiles/low-battery.ogg
#defaults.path = $${L_SHAREDIR}/lumina-desktop/

#conf.path = $${L_ETCDIR}

#Now do any OS-specific defaults (if available)
#First see if there is a known OS override first
#!isEmpty(DEFAULT_SETTINGS){
#  message("Installing defaults settings for OS: $${DEFAULT_SETTINGS}")
#  OS=$${DEFAULT_SETTINGS}
#}
#exists("$$PWD/defaults/luminaDesktop-$${OS}.conf"){
#  message(" -- Found OS-specific system config file: $${OS}");
#  conf.extra = cp $$PWD/defaults/luminaDesktop-$${OS}.conf $(INSTALL_ROOT)$${L_ETCDIR}/luminaDesktop.conf.dist
#}else{
#  conf.extra = cp $$PWD/defaults/luminaDesktop.conf $(INSTALL_ROOT)$${L_ETCDIR}/luminaDesktop.conf.dist
#}
#exists("$$PWD/defaults/desktop-background-$${OS}.jpg"){
#  message(" -- Found OS-specific background image: $${OS}");
#  defaults.extra = cp $$PWD/defaults/desktop-background-$${OS}.jpg $(INSTALL_ROOT)$${L_SHAREDIR}/lumina-desktop/desktop-background.jpg
#}else{
#  defaults.extra = cp $$PWD/defaults/desktop-background.jpg $(INSTALL_ROOT)$${L_SHAREDIR}/lumina-desktop/desktop-background.jpg
#}


#manpage.path=$${L_MANDIR}/man1/
#manpage.extra="$${MAN_ZIP} $$PWD/lumina-desktop.1 > $(INSTALL_ROOT)$${L_MANDIR}/man1/lumina-desktop.1.gz"

#INSTALLS += target
# desktop icons defaults conf fluxconf manpage

RESOURCES += \
    desktop.qrc

