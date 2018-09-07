# schematiceditor.pro

QT += widgets svg

TARGET = schematiceditor
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp \
    schematiceditor.cpp \
    element.cpp \
    schema.cpp \
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
    schema.h \
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
