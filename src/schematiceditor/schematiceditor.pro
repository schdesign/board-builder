# schematiceditor.pro

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = schematiceditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += array.cpp \
    arrayselector.cpp \
    circuitsymbol.cpp \
    device.cpp \
    deviceselector.cpp \
    element.cpp \
    function.cpp \
    main.cpp \
    packageselector.cpp \
    schematic.cpp \
    schematiceditor.cpp \
    symboleditor.cpp \
    text.cpp \
    types.cpp \
    unit.cpp

HEADERS += array.h \
    arrayimage.h \
    arrayselector.h \
    circuitsymbol.h \
    circuitsymbolimage.h \
    device.h \
    deviceselector.h \
    element.h \
    elementimage.h \
    exceptiondata.h \
    function.h \
    packageselector.h \
    schematic.h \
    schematiceditor.h \
    symboleditor.h \
    text.h \
    types.h \
    unit.h \
    unitimage.h

FORMS += arrayselector.ui \
    deviceselector.ui \
    packageselector.ui \
    schematiceditor.ui \
    symboleditor.ui

RESOURCES += schematiceditor.qrc \
    symboleditor.qrc
