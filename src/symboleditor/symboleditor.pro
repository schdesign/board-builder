# symboleditor.pro

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = symboleditor
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
        symboleditor.cpp

HEADERS += symboleditor.h

FORMS += symboleditor.ui
