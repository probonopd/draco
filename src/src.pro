TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop
desktop.depends = lib
