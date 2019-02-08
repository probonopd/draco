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
  DEFINES+=NO_XINPUT
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

#TRANSLATIONS =  i18n/lumina-config_af.ts \
#                i18n/lumina-config_ar.ts \
#                i18n/lumina-config_az.ts \
#                i18n/lumina-config_bg.ts \
#                i18n/lumina-config_bn.ts \
#                i18n/lumina-config_bs.ts \
#                i18n/lumina-config_ca.ts \
#                i18n/lumina-config_cs.ts \
#                i18n/lumina-config_cy.ts \
#                i18n/lumina-config_da.ts \
#                i18n/lumina-config_de.ts \
#                i18n/lumina-config_el.ts \
#                i18n/lumina-config_en_GB.ts \
#                i18n/lumina-config_en_ZA.ts \
#                i18n/lumina-config_en_AU.ts \
#                i18n/lumina-config_es.ts \
#                i18n/lumina-config_et.ts \
#                i18n/lumina-config_eu.ts \
#                i18n/lumina-config_fa.ts \
#                i18n/lumina-config_fi.ts \
#                i18n/lumina-config_fr.ts \
#                i18n/lumina-config_fr_CA.ts \
#                i18n/lumina-config_gl.ts \
#                i18n/lumina-config_he.ts \
#                i18n/lumina-config_hi.ts \
#                i18n/lumina-config_hr.ts \
#                i18n/lumina-config_hu.ts \
#                i18n/lumina-config_id.ts \
#                i18n/lumina-config_is.ts \
#                i18n/lumina-config_it.ts \
#                i18n/lumina-config_ja.ts \
#                i18n/lumina-config_ka.ts \
#                i18n/lumina-config_ko.ts \
#                i18n/lumina-config_lt.ts \
#                i18n/lumina-config_lv.ts \
#                i18n/lumina-config_mk.ts \
#                i18n/lumina-config_mn.ts \
#                i18n/lumina-config_ms.ts \
#                i18n/lumina-config_mt.ts \
#                i18n/lumina-config_nb.ts \
#                i18n/lumina-config_nl.ts \
#                i18n/lumina-config_pa.ts \
#                i18n/lumina-config_pl.ts \
#                i18n/lumina-config_pt.ts \
#                i18n/lumina-config_pt_BR.ts \
#                i18n/lumina-config_ro.ts \
#                i18n/lumina-config_ru.ts \
#                i18n/lumina-config_sk.ts \
#                i18n/lumina-config_sl.ts \
#                i18n/lumina-config_sr.ts \
#                i18n/lumina-config_sv.ts \
#                i18n/lumina-config_sw.ts \
#                i18n/lumina-config_ta.ts \
#                i18n/lumina-config_tg.ts \
#                i18n/lumina-config_th.ts \
#                i18n/lumina-config_tr.ts \
#                i18n/lumina-config_uk.ts \
#                i18n/lumina-config_uz.ts \
#                i18n/lumina-config_vi.ts \
#                i18n/lumina-config_zh_CN.ts \
#                i18n/lumina-config_zh_HK.ts \
#                i18n/lumina-config_zh_TW.ts \
#                i18n/lumina-config_zu.ts

#dotrans.path=$${L_SHAREDIR}/lumina-desktop/i18n/
#dotrans.extra=cd $$PWD/i18n && $${LRELEASE} -nounfinished *.ts && cp *.qm $(INSTALL_ROOT)$${L_SHAREDIR}/lumina-desktop/i18n/

#desktop.files=lumina-config.desktop
#desktop.path=$${L_SHAREDIR}/applications/

#manpage.path=$${L_MANDIR}/man1/
#manpage.extra="$${MAN_ZIP} $$PWD/lumina-config.1 > $(INSTALL_ROOT)$${L_MANDIR}/man1/lumina-config.1.gz"

#INSTALLS += target desktop manpage

#WITH_I18N{
#  INSTALLS += dotrans
#}
