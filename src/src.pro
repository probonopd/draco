TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop session launcher
desktop.depends += lib
session.depends += lib
launcher.depends += lib
