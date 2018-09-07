// array.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ARRAY_H
#define ARRAY_H

#include "arrayimage.h"
#include "types.h"
#include <QPainter>
#include <QString>
#include <QStringRef>
#include <QVector>

class ArraySymbol
{
public:
    bool showPinName;
    bool showPinNumber;
    Border border;
    int deltaY;
    int referenceTextX[2];  // left, right
    int referenceTextY[2];
    int refX;
    int refY;
    int type;
    int nameTextX[2];       // left bottom point of text
    int nameTextY[2];
    int pinNameTextX[2];
    int pinNameTextY[2];
    int pinNumberTextX[2];
    int pinNumberTextY[2];
    QString name;
    QString reference;
    std::vector<Line> lines;
    std::vector<Point> pins;

    QJsonObject toJson();
};

class Array
{
public:
    static std::map <int, ArraySymbol> symbols; // array nameID, array Symbol
    bool showPinName;
    bool showPinNumber;
    Border border;
    int center;
    int centerX;
    int centerY;
    int deltaY;
    int nameTextX;          // left bottom point of text
    int nameTextY;
    int number;             // number of elements
    int orientation;
    int pinNameTextX;
    int pinNameTextY;
    int pinNumberTextX;
    int pinNumberTextY;
    int referenceTextX;     // left bottom point of text
    int referenceTextY;
    int referenceType;      // X
    int refX;               // point of pin
    int refY;
    int type;               // CONNECTOR
    QString name;
    QString package;
    QString reference;      // X1
    std::vector<QString> pinName;
    std::vector<Line> lines;
    std::vector<Point> pins;

    Array() {}
    Array(int type, int number, int refX,
          int refY, int orientation);
    Array(const QJsonObject &object);
    ~Array() {}
    static void addSymbol(const QJsonValue &value);
    static QJsonObject writeSymbols();
    void draw(QPainter &painter);
    bool exist(int x, int y);
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY);
    QJsonObject toJson();
};

#endif  // ARRAY_H

