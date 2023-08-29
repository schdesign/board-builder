// unit.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef UNIT_H
#define UNIT_H

#include "types.h"
#include "unitimage.h"
#include <QPainter>
#include <QString>
#include <QVector>
#include <vector>

class UnitSymbol
{
public:
    QJsonObject toJson();

    Border border;
    int nameID;
    int number;
    int refX;
    int refY;
    std::vector<Ellipse> ellipses;
    std::vector<Line> lines;
};

class Unit
{
public:
    Unit() {}
    Unit(int deviceID, int number, int refX, int refY);
    Unit(int deviceID, const QJsonObject &object);
    static void addSymbol(const QJsonValue &value, int deviceID);
    void draw(QPainter &painter, int fontSize);
    bool exist(int x, int y);
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY);
    QJsonObject toJson();

    static std::map<int, UnitSymbol> symbols;  // unit nameID, unitSymbol
    Border border;
    int center;
    int centerX;
    int centerY;
    int deviceID;           // device nameID
    int number;             // unit number in device
    int refX;               // point of top left pin
    int refY;
    int symbolNameID;       // unit symbol nameID
    QString reference;      // D1.1
    std::vector<Ellipse> ellipses;
    std::vector<Line> lines;
};

#endif  // UNIT_H
