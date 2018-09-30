// circuitsymbol.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef CIRCUITSYMBOL_H
#define CIRCUITSYMBOL_H

#include "circuitsymbolimage.h"
#include <QJsonObject>
#include <QPainter>
#include <QString>
#include <QStringRef>
#include <QVector>

class CircuitSymbol
{
public:
    CircuitSymbol() {}
    CircuitSymbol(int type, int refX, int refY);
    CircuitSymbol(const QJsonObject &object);
    void draw(QPainter &painter);
    bool exist(int x, int y);
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY);
    void placeLines(const int (*symbolLines)[4]);
    QJsonObject toJson();

    int lines[16][4];   // lines number <= 16
    int arcs[4][4];     // arcs number <= 4
    int linesNumber;
    int arcsNumber;
    int type;           // GROUND
    int refX;           // reference point
    int refY;
    int centerX;
    int centerY;
    int center;
};

#endif  // CIRCUITSYMBOL_H
