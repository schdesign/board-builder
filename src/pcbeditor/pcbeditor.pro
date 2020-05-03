# pcbeditor.pro

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pcbeditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

include(../common/common.pri)

SOURCES += board.cpp \
    element.cpp \
    elementoptions.cpp \
    function.cpp \
    jumperselector.cpp \
    layers.cpp \
    main.cpp \
    packageeditor.cpp \
    pcbeditor.cpp \
    pcbtypes.cpp \
    router.cpp \
    text.cpp \
    track.cpp \
    viaoptions.cpp

HEADERS += board.h \
    element.h \
    elementoptions.h \
    exceptiondata.h \
    function.h \
    jumperselector.h \
    layers.h \
    packageeditor.h \
    pcbeditor.h \
    pcbtypes.h \
    router.h \
    text.h \
    track.h \
    viaoptions.h

FORMS += packageeditor.ui \
    elementoptions.ui \
    jumperselector.ui \
    pcbeditor.ui \
    viaoptions.ui

RESOURCES += packageeditor.qrc \
    pcbeditor.qrc
