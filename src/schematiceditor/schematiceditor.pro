# schematiceditor.pro

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = schematiceditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    schematiceditor.cpp \
    element.cpp \
    schematic.cpp \
    types.cpp \
    symbol.cpp \
    unit.cpp \
    device.cpp \
    text.cpp \
    deviceselector.cpp \
    function.cpp \
    array.cpp \
    arrayselector.cpp \
    packageselector.cpp

HEADERS += schematiceditor.h \
    element.h \
    exceptiondata.h \
    schematic.h \
    types.h \
    elementimage.h \
    symbol.h \
    symbolimage.h \
    unit.h \
    unitimage.h \
    device.h \
    text.h \
    deviceselector.h \
    function.h \
    array.h \
    arrayimage.h \
    arrayselector.h \
    packageselector.h

FORMS += schematiceditor.ui \
    arrayselector.ui \
    deviceselector.ui \
    packageselector.ui

RESOURCES += schematiceditor.qrc
