include(../MedicycCS2.pri)

SUBDIRS = src exe gui

gui.depends = src
exe.depends = src

RESOURCES = resources.qrc
