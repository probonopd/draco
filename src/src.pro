TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop session utils settings launcher
desktop.depends = lib
session.depends = lib
utils.depends = lib
settings.depends = lib
