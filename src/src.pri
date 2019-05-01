
DESKTOP_TARGET = "draco"
DESKTOP_TARGET_NAME = "Draco"
DESKTOP_DOMAIN = "dracolinux.org"

DESKTOP_MAJOR = 0
DESKTOP_MINOR = 9
DESKTOP_PATCH = 0

VERSION = $${DESKTOP_MAJOR}.$${DESKTOP_MINOR}.$${DESKTOP_PATCH}
DEFINES += DESKTOP_APP=\"\\\"$${DESKTOP_TARGET}\\\"\"
DEFINES += DESKTOP_APP_NAME=\"\\\"$${DESKTOP_TARGET_NAME}\\\"\"
DEFINES += DESKTOP_APP_VERSION=\"\\\"$${VERSION}\\\"\"
DEFINES += DESKTOP_APP_GIT=\"\\\"$${GIT}\\\"\"
DEFINES += DESKTOP_APP_DOMAIN=\"\\\"$${DESKTOP_DOMAIN}\\\"\"

QT += core dbus

isEmpty(PREFIX) {
    PREFIX = /usr/local
    isEmpty(ETCDIR) : ETCDIR = $${PREFIX}/etc
}
isEmpty(LIBDIR) : LIBDIR = $$PREFIX/lib$${LIBSUFFIX}
isEmpty(DOCDIR) : DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR) : MANDIR = $$PREFIX/share/man
isEmpty(ETCDIR) : ETCDIR = /etc
isEmpty(XDGDIR) : XDGDIR = $${ETCDIR}/xdg
isEmpty(XINITDIR) : XINITDIR = $${ETCDIR}/X11/xinit
isEmpty(XSESSIONSDIR) : XSESSIONSDIR = $${PREFIX}/share/xsessions

CONFIG += link_pkgconfig
QT_CONFIG -= no-pkg-config

CONFIG(release, debug|release) {
    #!CONFIG(no_static): CONFIG += staticlib
    DEFINES += QT_NO_DEBUG_OUTPUT
}

!CONFIG(staticlib): QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

#DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# check for adwaita
PKGCONFIG += adwaita-icon-theme
ICON_VERSION = $$system(pkg-config --modversion adwaita-icon-theme)
greaterThan(ICON_VERSION, 3.28.0): warning("ADWAITA $${ICON_VERSION} IS BROKEN, PLEASE USE A OLDER VERSION IF POSSIBLE!")
