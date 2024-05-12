include(../MedicycCS2.pri)

SUBDIRS = src tests gui
gui.depends = src
tests.depends = src

RESOURCES = resources.qrc
