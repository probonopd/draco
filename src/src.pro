TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop desktop-settings
desktop.depends = lib
desktop-settings.depends = lib
