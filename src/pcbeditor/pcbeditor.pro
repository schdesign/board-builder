# pcbeditor.pro

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pcbeditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    pcbeditor.cpp \
    element.cpp \
    types.cpp \
    text.cpp \
    board.cpp \
    layers.cpp \
    track.cpp \
    pcbtypes.cpp \
    router.cpp \
    function.cpp

HEADERS += pcbeditor.h \
    element.h \
    exceptiondata.h \
    types.h \
    text.h \
    board.h \
    layers.h \
    track.h \
    pcbtypes.h \
    router.h \
    function.h

FORMS += pcbeditor.ui

RESOURCES += pcbeditor.qrc
