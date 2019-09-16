// element.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ELEMENT_H
#define ELEMENT_H

#include "elementimage.h"
#include "types.h"
#include <QPainter>
#include <QString>
#include <QStringRef>
#include <QVector>

class ElementSymbol
{
public:
    QJsonObject toJson();

    static constexpr int orientations = 8;
    bool mirror;
    Border border;
    int referenceTextX[orientations];  // 0...3: up, right, down, left
    int referenceTextY[orientations];  // 4...7: up mirror, right mirror, down mirror, left mirror
    int refX;
    int refY;
    int type;
    int valueTextX[orientations];      // left bottom point of text
    int valueTextY[orientations];
    QString name;
    QString reference;
    std::vector<Arc> arcs;
    std::vector<Line> lines;
    std::vector<Point> pins;
};

class Element
{
public:
    Element() {}
    Element(int type, int refX, int refY,
            int orientation, QString value = "1");
    Element(const QJsonObject &object);
    static void addSymbol(const QJsonValue &value);
    static QJsonObject writeSymbols();
    void draw(QPainter &painter);
    bool exist(int x, int y);
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY);
    QJsonObject toJson();

    static std::map <int, ElementSymbol> symbols;  // element nameID, elementSymbol
    static constexpr int orientations = 8;
    bool mirror;
    Border border;
    int center;
    int centerX;
    int centerY;
    int orientation;        // UP
    int referenceTextX;     // left bottom point of text
    int referenceTextY;
    int referenceType;      // R
    int refX;               // point of pin
    int refY;
    int type;               // RESISTOR
    int valueTextX;         // left bottom point of text
    int valueTextY;
    QString package;
    QString reference;      // R1    
    QString value;          // 1K
    std::vector<Arc> arcs;
    std::vector<Line> lines;
    std::vector<Point> pins;
};

#endif  // ELEMENT_H
