// symbol.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SYMBOL_H
#define SYMBOL_H

#include "symbolimage.h"
#include <QJsonObject>
#include <QPainter>
#include <QString>
#include <QStringRef>
#include <QVector>

class Symbol
{
public:
    int lines[16][4];           // lines number <= 16
    int arcs[4][4];             // arcs number <= 4
    int linesNumber;
    int arcsNumber;
    int type;                   // GROUND
    int refX;                   // reference point
    int refY;
    int centerX;
    int centerY;
    int center;

    Symbol() {}
    Symbol(int type, int refX, int refY);
    Symbol(const QJsonObject &object);
    ~Symbol() {}
    void draw(QPainter &painter);
    bool exist(int x, int y);    
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY);
    void placeLines(const int (*image)[4]);
    QJsonObject toJson();
};

#endif  // SYMBOL_H
