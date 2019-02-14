include($${top_srcdir}/src/src.pri)

QT += network widgets x11extras

TARGET = start-$${DESKTOP_TARGET}-desktop
TEMPLATE = app

DESTDIR = $${top_builddir}/bin
OBJECTS_DIR = $${DESTDIR}/.obj_session
MOC_DIR = $${DESTDIR}/.moc_session
RCC_DIR = $${DESTDIR}/.qrc_session

INCLUDEPATH += $${top_srcdir}/src/lib
LIBS += -L$${top_builddir}/lib$${LIBSUFFIX} -l$${DESKTOP_TARGET_NAME}Desktop
!CONFIG(staticlib): QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

SOURCES += main.cpp session.cpp
HEADERS  += session.h

#manpage.path=$${L_MANDIR}/man8/
#manpage.extra="$${MAN_ZIP} $$PWD/start-lumina-desktop.8 > $(INSTALL_ROOT)$${L_MANDIR}/man8/start-lumina-desktop.8.gz"

#INSTALLS += target manpage
