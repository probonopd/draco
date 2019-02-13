include($${top_srcdir}/src/src.pri)

QT += widgets

TARGET  = $${DESKTOP_TARGET}-archiver
TEMPLATE = app

DESTDIR = $${top_builddir}/bin
OBJECTS_DIR = $${DESTDIR}/.obj_archiver
MOC_DIR = $${DESTDIR}/.moc_archiver
RCC_DIR = $${DESTDIR}/.qrc_archiver

INCLUDEPATH += $${top_srcdir}/src/lib
LIBS += -L$${top_builddir}/lib$${LIBSUFFIX} -l$${DESKTOP_TARGET_NAME}Desktop
!CONFIG(staticlib): QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

HEADERS += MainUI.h TarBackend.h
SOURCES += main.cpp MainUI.cpp TarBackend.cpp

FORMS = MainUI.ui

#dotrans.path=$${L_SHAREDIR}/lumina-desktop/i18n/
#dotrans.extra=cd $$PWD/i18n && $${LRELEASE} -nounfinished *.ts && cp *.qm $(INSTALL_ROOT)$${L_SHAREDIR}/lumina-desktop/i18n/

#desktop.files=lumina-archiver.desktop
#desktop.path=$${L_SHAREDIR}/applications/

#link.path=$${L_BINDIR}
#link.extra=ln -sf lumina-archiver $(INSTALL_ROOT)$${L_BINDIR}/lpac

#manpage.path=$${L_MANDIR}/man1/
#manpage.extra="$${MAN_ZIP} $$PWD/lumina-archiver.1 > $(INSTALL_ROOT)$${L_MANDIR}/man1/lumina-archiver.1.gz"

#INSTALLS += target desktop manpage

#WITH_I18N{
#  INSTALLS += dotrans
#}
