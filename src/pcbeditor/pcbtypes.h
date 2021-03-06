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
    int number;               // net number
    std::vector<Point> pads;  // x: element, y: pad
};

class Polygon
{
public:
    Polygon() {}
    Polygon(const QJsonValue &value);
    bool center(int &x, int &y);
    void draw(QPainter &painter, double scale, QBrush brush);
    void fromJson(const QJsonValue &value);
    bool hasInnerPoint(int x, int y);
    QJsonObject toJson();

    bool fill;
    int net;
    std::vector<Point> points;
};

// Circle
class Via
{
public:
    Via() {}
    Via(int x, int y);
    Via(const QJsonValue &value);
    void draw(QPainter &painter, int layerNumber, double scale, int space = 0);
    bool exist(int x_, int y_);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int diameter;
    int innerDiameter;
    int net;          // net number
    int x;            // center
    int y;
};

// Horizontal or vertical lines
bool crossLines2(const double line1[], const double line2[], double &x, double &y);
bool crossPoint2(const double line[], double x, double y);
bool joinLines2(double line1[], const double line2[]);

#endif  // PCB_TYPES_H
