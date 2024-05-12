#
# This file contains the main variables settings to build MedicycCS1 on all supported platforms.
# It is included in all the *.pro of the project !
#
message( "=> Global Medicyc Control System v2 settings !" )

#
# qt : Qt project
# warn_on : Maximize warnings
# rtti : Run-time type information
# stl : Standard Template Library
# c++14 : C++14 Standard
# c++17 : C++17 Standard
CONFIG += qt warn_on rtti stl c++17 c++1z

QT += core

RASPBERRYPI = $$system($$PWD/Scripts/onapi.sh)

TEMPLATE = subdirs

# Unix settings (i.e. Linux or macOS X)
unix {
    message( " -> Settings for Unix system's build..." )
}

# Linux settings
linux {
    message( " -> Settings for Linux build..." )
}

# macOS X settings
macx {
    message( " -> Settings for macOS X build..." )
    QMAKE_CXXFLAGS += -Wc++17-extensions
}

# Windows settings
windows {
    message( " -> Settings for Windows build..." )
}

