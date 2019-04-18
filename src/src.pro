TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop session utils settings
desktop.depends = lib
session.depends = lib
utils.depends = lib
settings.depends = lib
