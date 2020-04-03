// types.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef TYPES_H
#define TYPES_H

#include <QJsonObject>
#include <QJsonValue>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

constexpr double minValue = 1E-9;

class Arc
{
public:
    Arc() {}
    Arc(int x, int y, int w, int h, int startAngle, int spanAngle):
        x(x), y(y), w(w), h(h),
        startAngle(startAngle), spanAngle(spanAngle) { }
    Arc(const QJsonValue &value);
    void clear();
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int x;
    int y;
    int w;
    int h;
    int startAngle;  // angle = 0 degree: 3 o'clock position
    int spanAngle;   // angle > 0 degree: counter-clockwise direction
};

class Border
{
public:
    Border() {}
    Border(int leftX, int topY, int rightX, int bottomY):
        leftX(leftX), topY(topY), rightX(rightX), bottomY(bottomY) {}
    Border(const QJsonValue &value);
    void clear();
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int leftX;
    int topY;
    int rightX;
    int bottomY;
};

class Ellipse
{
public:
    Ellipse() {}
    Ellipse(int x, int y, int w, int h):
        x(x), y(y), w(w), h(h) {}
    Ellipse(const QJsonValue &value);
    void clear();
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int x;
    int y;
    int w;
    int h;
};

class Line
{
public:
    Line() {}
    Line(int x1, int y1, int x2, int y2):
        x1(x1), y1(y1), x2(x2), y2(y2) {}
    Line(const QJsonValue &value);
    void clear();
    bool crossLine(const Line &line, int &x, int &y);
    bool crossPoint(int x, int y) const;
    void fromJson(const QJsonValue &value);
    bool join(const Line &line);
    int length() const;
    QJsonObject toJson();

    int x1;
    int y1;
    int x2;
    int y2;
};

class Point
{
public:
    Point() {}
    Point(int x, int y): x(x), y(y) {}
    Point(const QJsonValue &value);
    void clear();
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();
    bool operator ==(const Point &point) const;
    bool operator <(const Point &point) const;

    int x;
    int y;
};

#endif  // TYPES_H
