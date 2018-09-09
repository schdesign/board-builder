# packageeditor.pro

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = packageeditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
        packageeditor.cpp

HEADERS += packageeditor.h

FORMS += packageeditor.ui
