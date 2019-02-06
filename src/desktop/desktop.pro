#include($${PWD}/../../OS-detect.pri)

QT += network widgets x11extras concurrent svg

TARGET = draco-desktop
#target.path = $${L_BINDIR}

#include all the special classes from the Lumina tree
include(../libLumina/ResizeMenu.pri)
include(../libLumina/LDesktopUtils.pri) #includes LUtils and LOS
include(../libLumina/LuminaXDG.pri)
include(../libLumina/LuminaX11.pri)
include(../libLumina/LuminaSingleApplication.pri)
include(../libLumina/LuminaThemes.pri)
include(../libLumina/LIconCache.pri)

LIBS = -L$${PWD}/core/libLumina
INCLUDEPATH = $${PWD}/core/libLumina

TEMPLATE = app

SOURCES += main.cpp \
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
	desktop-plugins/LDPlugin.cpp


HEADERS  += Globals.h \
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
	JsonMenu.h

FORMS    += SystemWindow.ui


#Now include all the files for the various plugins
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
