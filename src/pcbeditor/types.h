// types.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef TYPES_H
#define TYPES_H

#include <QJsonObject>
#include <QJsonValue>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

const double minValue = 1E-9;

class Border
{
public:
    int leftX;
    int topY;
    int rightX;
    int bottomY;

    Border() {}
    Border(int leftX, int topY, int rightX, int bottomY):
         leftX(leftX), topY(topY), rightX(rightX), bottomY(bottomY) {}
    Border(const QJsonValue &value);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();
};

class Ellipse
{
public:
    int x;
    int y;
    int w;
    int h;

    Ellipse() {}
    Ellipse(int x, int y, int w, int h):
        x(x), y(y), w(w), h(h) {}
    Ellipse(const QJsonValue &value);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();
};

class Line
{
public:
    int x1;
    int y1;
    int x2;
    int y2;

    Line() {}
    Line(int x1, int y1, int x2, int y2):
         x1(x1), y1(y1), x2(x2), y2(y2) {}
    Line(const QJsonValue &value);
    bool crossPoint(int x, int y);
    bool join(const Line &line);
    QJsonObject toJson();
};

class Point
{
public:
    int x;
    int y;

    Point() {}
    Point(int x, int y): x(x), y(y) {}
    Point(const QJsonValue &value);
    QJsonObject toJson();
    bool operator ==(const Point &point) const;
    bool operator <(const Point &point) const;
};

#endif  // TYPES_H
