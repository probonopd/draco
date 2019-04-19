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

SOURCES += main.cpp session.cpp
HEADERS  += session.h

CONFIG(staticlib) : RESOURCES += $${top_srcdir}/share/share.qrc

target.path = $${PREFIX}/bin
xinit.path = $${XINITDIR}
xinit.files = $${top_srcdir}/share/xinit/xinitrc.draco
xsessions.path = $${XSESSIONSDIR}
xsessions.files = $${top_srcdir}/share/xsessions/draco.desktop

INSTALLS += target xinit xsessions
