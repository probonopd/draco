TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop session utils
desktop.depends = lib
session.depends = lib
utils.depends = lib
