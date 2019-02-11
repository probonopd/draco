include($${top_srcdir}/src/src.pri)

QT += widgets x11extras network

TARGET = $${DESKTOP_TARGET}-desktop-settings
TEMPLATE = app

DESTDIR = $${top_builddir}/bin
OBJECTS_DIR = $${DESTDIR}/.obj_desktop-settings
MOC_DIR = $${DESTDIR}/.moc_desktop-settings
RCC_DIR = $${DESTDIR}/.qrc_desktop-settings

INCLUDEPATH += $${top_srcdir}/src/lib
LIBS += -L$${top_builddir}/lib$${LIBSUFFIX} -l$${DESKTOP_TARGET_NAME}Desktop
!CONFIG(staticlib): QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

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

#NO_XINPUT{
#  DEFINES+=NO_XINPUT
#}else{
#  include(../../core/libLumina/LInputDevice.pri)
#}

SOURCES += main.cpp \
	   mainWindow.cpp \
	   LPlugins.cpp \
	   ColorDialog.cpp \
	   ThemeDialog.cpp \
	   GetPluginDialog.cpp \
           #ScriptDialog.cpp \
	   PanelWidget.cpp

HEADERS  += mainWindow.h \
		LPlugins.h \
		AppDialog.h \
		ColorDialog.h \
		ThemeDialog.h \
		GetPluginDialog.h \
                #ScriptDialog.h \
		PanelWidget.h

FORMS    += mainWindow.ui \
		AppDialog.ui \
		ColorDialog.ui \
		ThemeDialog.ui \
		GetPluginDialog.ui \
                #ScriptDialog.ui \
		PanelWidget.ui

include("pages/pages.pri")


#desktop.files=lumina-config.desktop
#desktop.path=$${L_SHAREDIR}/applications/

#manpage.path=$${L_MANDIR}/man1/
#manpage.extra="$${MAN_ZIP} $$PWD/lumina-config.1 > $(INSTALL_ROOT)$${L_MANDIR}/man1/lumina-config.1.gz"

#INSTALLS += target desktop manpage

#WITH_I18N{
#  INSTALLS += dotrans
#}
