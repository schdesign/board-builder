# pcbeditor.pro

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pcbeditor
TEMPLATE = app

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

include(../common/common.pri)

SOURCES += board.cpp \
    copperbalance.cpp \
    element.cpp \
    function.cpp \
    globaloptions.cpp \
    jumperselector.cpp \
    layers.cpp \
    localoptions.cpp \
    main.cpp \
    packageeditor.cpp \
    pcbeditor.cpp \
    pcbtypes.cpp \
    router.cpp \
    text.cpp \
    track.cpp

HEADERS += board.h \
    copperbalance.h \
    element.h \
    exceptiondata.h \
    function.h \
    globaloptions.h \
    jumperselector.h \
    layers.h \
    localoptions.h \
    packageeditor.h \
    pcbeditor.h \
    pcbtypes.h \
    router.h \
    text.h \
    track.h

FORMS += globaloptions.ui \
    copperbalance.ui \
    jumperselector.ui \
    localoptions.ui \
    packageeditor.ui \
    pcbeditor.ui

RESOURCES += packageeditor.qrc \
    pcbeditor.qrc
