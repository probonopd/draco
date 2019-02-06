include("$${PWD}/../../OS-detect.pri")

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = lumina-xconfig
target.path = $${L_BINDIR}

TEMPLATE = app

#include all the special classes from the Lumina tree
include(../../core/libLumina/LUtils.pri) #includes LUtils
include(../../core/libLumina/LuminaXDG.pri)
include(../../core/libLumina/LuminaSingleApplication.pri)
include(../../core/libLumina/LuminaThemes.pri)

SOURCES += main.cpp \
		mainUI.cpp \
		ScreenSettings.cpp

HEADERS  += mainUI.h \
		ScreenSettings.h

FORMS    += mainUI.ui


TRANSLATIONS =  i18n/lumina-xconfig_af.ts \
                i18n/lumina-xconfig_ar.ts \
                i18n/lumina-xconfig_az.ts \
                i18n/lumina-xconfig_bg.ts \
                i18n/lumina-xconfig_bn.ts \
                i18n/lumina-xconfig_bs.ts \
                i18n/lumina-xconfig_ca.ts \
                i18n/lumina-xconfig_cs.ts \
                i18n/lumina-xconfig_cy.ts \
                i18n/lumina-xconfig_da.ts \
                i18n/lumina-xconfig_de.ts \
                i18n/lumina-xconfig_el.ts \
                i18n/lumina-xconfig_en_GB.ts \
                i18n/lumina-xconfig_en_ZA.ts \
                i18n/lumina-xconfig_en_AU.ts \
                i18n/lumina-xconfig_es.ts \
                i18n/lumina-xconfig_et.ts \
                i18n/lumina-xconfig_eu.ts \
                i18n/lumina-xconfig_fa.ts \
                i18n/lumina-xconfig_fi.ts \
                i18n/lumina-xconfig_fr.ts \
                i18n/lumina-xconfig_fr_CA.ts \
                i18n/lumina-xconfig_gl.ts \
                i18n/lumina-xconfig_he.ts \
                i18n/lumina-xconfig_hi.ts \
                i18n/lumina-xconfig_hr.ts \
                i18n/lumina-xconfig_hu.ts \
                i18n/lumina-xconfig_id.ts \
                i18n/lumina-xconfig_is.ts \
                i18n/lumina-xconfig_it.ts \
                i18n/lumina-xconfig_ja.ts \
                i18n/lumina-xconfig_ka.ts \
                i18n/lumina-xconfig_ko.ts \
                i18n/lumina-xconfig_lt.ts \
                i18n/lumina-xconfig_lv.ts \
                i18n/lumina-xconfig_mk.ts \
                i18n/lumina-xconfig_mn.ts \
                i18n/lumina-xconfig_ms.ts \
                i18n/lumina-xconfig_mt.ts \
                i18n/lumina-xconfig_nb.ts \
                i18n/lumina-xconfig_nl.ts \
                i18n/lumina-xconfig_pa.ts \
                i18n/lumina-xconfig_pl.ts \
                i18n/lumina-xconfig_pt.ts \
                i18n/lumina-xconfig_pt_BR.ts \
                i18n/lumina-xconfig_ro.ts \
                i18n/lumina-xconfig_ru.ts \
                i18n/lumina-xconfig_sk.ts \
                i18n/lumina-xconfig_sl.ts \
                i18n/lumina-xconfig_sr.ts \
                i18n/lumina-xconfig_sv.ts \
                i18n/lumina-xconfig_sw.ts \
                i18n/lumina-xconfig_ta.ts \
                i18n/lumina-xconfig_tg.ts \
                i18n/lumina-xconfig_th.ts \
                i18n/lumina-xconfig_tr.ts \
                i18n/lumina-xconfig_uk.ts \
                i18n/lumina-xconfig_uz.ts \
                i18n/lumina-xconfig_vi.ts \
                i18n/lumina-xconfig_zh_CN.ts \
                i18n/lumina-xconfig_zh_HK.ts \
                i18n/lumina-xconfig_zh_TW.ts \
                i18n/lumina-xconfig_zu.ts

dotrans.path=$${L_SHAREDIR}/lumina-desktop/i18n/
dotrans.extra=cd $$PWD/i18n && $${LRELEASE} -nounfinished *.ts && cp *.qm $(INSTALL_ROOT)$${L_SHAREDIR}/lumina-desktop/i18n/

desktop.files=lumina-xconfig.desktop
desktop.path=$${L_SHAREDIR}/applications/

manpage.path=$${L_MANDIR}/man1/
manpage.extra="$${MAN_ZIP} $$PWD/lumina-xconfig.1 > $(INSTALL_ROOT)$${L_MANDIR}/man1/lumina-xconfig.1.gz"

INSTALLS += target desktop manpage

WITH_I18N{
  INSTALLS += dotrans
}
