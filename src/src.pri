DESKTOP_TARGET = "draco"
DESKTOP_TARGET_NAME = "Draco"
DESKTOP_DOMAIN = "dracolinux.org"

DESKTOP_MAJOR = 1
DESKTOP_MINOR = 0
DESKTOP_PATCH = 0

VERSION = $${DESKTOP_MAJOR}.$${DESKTOP_MINOR}.$${DESKTOP_PATCH}

isEmpty(PREFIX) {
    PREFIX = /usr/local
    isEmpty(ETCDIR) : ETCDIR = $${PREFIX}/etc
}
isEmpty(LIBDIR) : LIBDIR = $$PREFIX/lib$${LIBSUFFIX}
isEmpty(DOCDIR) : DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR) : MANDIR = $$PREFIX/share/man
isEmpty(ETCDIR) : ETCDIR = /etc
isEmpty(XDGDIR) : XDGDIR = $${ETCDIR}/xdg

CONFIG += link_pkgconfig
QT_CONFIG -= no-pkg-config

CONFIG(release, debug|release) : DEFINES += QT_NO_DEBUG_OUTPUT

#DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0