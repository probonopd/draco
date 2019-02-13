TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop desktop-settings utils
desktop.depends = lib
desktop-settings.depends = lib
utils.depends = lib
