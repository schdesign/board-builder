# pcbeditor.pro

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pcbeditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += board.cpp \
    element.cpp \
    function.cpp \
    layers.cpp \
    main.cpp \
    packageeditor.cpp \
    pcbeditor.cpp \
    pcbtypes.cpp \
    router.cpp \
    text.cpp \
    track.cpp \
    types.cpp

HEADERS += board.h \
    element.h \
    exceptiondata.h \
    function.h \
    layers.h \
    packageeditor.h \
    pcbeditor.h \
    pcbtypes.h \
    router.h \
    text.h \
    track.h \
    types.h

FORMS += packageeditor.ui \
    pcbeditor.ui

RESOURCES += packageeditor.qrc \
    pcbeditor.qrc
