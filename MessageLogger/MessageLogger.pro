include(../MedicycCS2.pri)

SUBDIRS = gui src
gui.depends = src

RESOURCES = resources.qrc
