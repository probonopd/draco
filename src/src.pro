TEMPLATE = subdirs
CONFIG -= order
SUBDIRS+= lib desktop session launcher
# utils
*.depends += lib
#session.depends += lib
#launcher.depends += lib
#utils.depends += lib
