// track.h
// Copyright (C) 2018 Alexander Karpeko

#include "track.h"
#include "types.h"
#include <cmath>

Segment::Segment()
{
    clear();
}

Segment::Segment(int x1, int y1, int x2, int y2, int net, int width):
    net(net), type(LINE), width(width), x1(x1), y1(y1), x2(x2), y2(y2)
{
    radius = 0;
    spanAngle = 0;
    startAngle = 0;
    x0 = 0;
    y0 = 0;
}

Segment::Segment(int x0, int y0, int radius, int startAngle,
                 int spanAngle, int net, int width):
    net(net), radius(radius), spanAngle(spanAngle),
    startAngle(startAngle), type(ARC), width(width), x0(x0), y0(y0)
{
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

Segment::Segment(const QJsonValue &value)
{
    fromJson(value);
}

void Segment::clear()
{
    net = 0;
    radius = 0;
    spanAngle = 0;
    startAngle = 0;
    type = 0;
    width = 0;
    x0 = 0;
    y0 = 0;
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

bool Segment::crossPoint(int x, int y)
{
    const double pi = acos(-1);
    double w2 = 0.5 * width;

    if (type == LINE) {
        double d1 = hypot((x - x1), (y - y1));
        double d2 = hypot((x - x2), (y - y2));
        if (d1 < w2 || d2 < w2)
            return true;
        double length = hypot((x1 - x2), (y1 - y2));
        double dmax = hypot(length, w2);
        if (d1 > dmax || d2 > dmax)
            return false;
        if (x1 == x2) {
            if (abs(x - x1) < w2)
                return true;
            return false;
        }
        double a = (y1 - y2) / (x2 - x1);
        double c = -a * x1 - y1;
        double d = fabs(a * x + y + c) / hypot(a, 1);
        if (d < w2)
            return true;
    }

    if (type == ARC) {
        double d = hypot((x - x0), (y - y0));
        double angle = 0;
        double sa = fmod(startAngle, 360);
        if (sa < 0)
            sa = 360 + sa;
        double sp = fmod(spanAngle, 360);
        double ea = fmod(sa + sp, 360);
        if (d > 0) {  // angle: 0...360 degrees
            angle = (180 / pi) * asin(-(y - y0) / d);
            if (x < x0)
                angle = 180 - angle;
            if (x > x0 && y > y0)
                angle = 360 + angle;
        }
        bool cross0 = sa + sp < 0;
        bool cross360 = sa + sp > 360;
        bool isAngleInRange = ((!cross0 && !cross360 && angle > sa && angle < ea) ||
                               (cross0 && (angle < sa || angle > 360 + ea)) ||
                               (cross360 && (angle > sa || angle < ea)));
        bool isDistanceInRange = d > radius - w2 && d < radius + w2;
        if (isAngleInRange && isDistanceInRange)
            return true;
    }

    return false;
}

void Segment::fromJson(const QJsonValue &value)
{
    clear();

    QJsonObject object = value.toObject();

    type = object["type"].toInt();

    if (type == LINE) {
        x1 = object["x1"].toInt();
        y1 = object["y1"].toInt();
        x2 = object["x2"].toInt();
        y2 = object["y2"].toInt();
    }

    if (type == ARC) {
        radius = object["radius"].toInt();
        spanAngle = object["spanAngle"].toInt();
        startAngle = object["startAngle"].toInt();
        x0 = object["x0"].toInt();
        y0 = object["y0"].toInt();
    }

    net = object["net"].toInt();
    width = object["width"].toInt();
}

bool Segment::hasCommonEndPoint(const Segment &s, int &x, int &y)
{
    if (type != LINE || s.type != LINE)
        return false;

    // Zero length
    if ((x1 == x2 && y1 == y2) || (s.x1 == s.x2 && s.y1 == s.y2))
        return false;

    Line line(x1, y1, x2, y2);

    // Common point is not single
    if ((x1 == s.x1 && y1 == s.y1 && line.crossPoint(s.x2, s.y2)) ||
        (x1 == s.x2 && y1 == s.y2 && line.crossPoint(s.x1, s.y1)))
        return false;

    if ((x1 == s.x1 && y1 == s.y1) || (x1 == s.x2 && y1 == s.y2)) {
        x = x1;
        y = y1;
        return true;
    }

    if ((x2 == s.x1 && y2 == s.y1) || (x2 == s.x2 && y2 == s.y2)) {
        x = x2;
        y = y2;
        return true;
    }

    return false;
}

double Segment::length()
{
    const double pi = acos(-1);
    double l = 0;

    if (type == LINE)
        l = hypot(x1 - x2, y1 - y2);

    if (type == ARC)
        l = fabs(2 * pi * radius * spanAngle / 360);

    return l;
}

QJsonObject Segment::toJson()
{
    if (type == LINE) {
        QJsonObject object
        {
            {"net", net},
            {"type", type},
            {"width", width},
            {"x1", x1},
            {"y1", y1},
            {"x2", x2},
            {"y2", y2}
        };
        return object;
    }

    if (type == ARC) {
        QJsonObject object
        {
            {"net", net},
            {"radius", radius},
            {"spanAngle", spanAngle},
            {"startAngle", startAngle},
            {"type", type},
            {"width", width},
            {"x0", x0},
            {"y0", y0}
        };
        return object;
    }

    QJsonObject object;
    return object;
}

/*
void Track::addSegment(int x1, int y1, int x2, int y2)
{

}

void Track::clear()
{
    curves.clear();
}

void Track::deleteSegment(int x1, int y1, int x2, int y2)
{

}

void Track::draw(QPainter &painter)
{

}

void Track::improve()
{

}

// improve curve and reduce segment number
void Track::improveCurve(Curve &curve)
{
    int last = curve.size() - 1;
    double x1 = curve[0].x[0];
    double y1 = curve[0].y[0];
    double x2 = curve[last].x[3];
    double y2 = curve[last].y[3];

    for (uint i = 0; i < curve.size(); i++) {

    }
}
*/
