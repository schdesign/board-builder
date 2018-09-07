// pcbtypes.h
// Copyright (C) 2018 Alexander Karpeko
// Coordinate unit: 1 micrometer

#ifndef PCB_TYPES_H
#define PCB_TYPES_H

#include "types.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>
#include <vector>

class Net
{
public:
    int number;                 // net number
    std::vector <Point> pads;   // x: element, y: pad
};

class Polygon
{
public:
    bool fill;
    int net;
    std::vector <Point> points;

    Polygon() {}
    Polygon(const QJsonValue &value);
    void draw(QPainter &painter, double scale, QBrush brush);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();
};

// Circle
class Via
{
public:
    int innerRadius;
    int net;            // net number
    int outerRadius;
    int x;              // center
    int y;

    Via() {}
    Via(const QJsonValue &value);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();
};

// Horizontal or vertical lines
bool crossLines2(const double line1[], const double line2[], double &x, double &y);
bool crossPoint2(const double line[], double x, double y);
bool joinLines2(double line1[], const double line2[]);

#endif  // PCB_TYPES_H
