// track.h
// Copyright (C) 2018 Alexander Karpeko

#include "function.h"
#include "track.h"
#include "types.h"
#include <algorithm>
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
    init();
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
        int angle = 0;
        int sa = startAngle % 360;
        int sp = spanAngle % 360;
        if (sp < 0) {
            sa += sp;
            sp = -sp;
        }
        if (sa < 0)
            sa = 360 + sa;
        int ea = (sa + sp) % 360;
        if (d > 0) {  // angle: 0...360 degrees
            angle = lround((180 / pi) * asin(-(y - y0) / d));
            if (x < x0)
                angle = 180 - angle;
            if (x > x0 && y > y0)
                angle = 360 + angle;
        }
        bool cross0 = sa + sp < 0;
        bool cross360 = sa + sp >= 360;
        bool isAngleInRange = ((!cross0 && !cross360 && angle >= sa && angle <= ea) ||
                               (cross0 && (angle <= sa || angle >= 360 + ea)) ||
                               (cross360 && (angle >= sa || angle <= ea)));
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

    init();
}

bool Segment::hasCommonEndPoint(const Segment &s, int &x, int &y)
{
    if (type != LINE || s.type != LINE)
        return false;

    if (length() == 0 || s.length() == 0)
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

void Segment::init()
{
    const double pi = acos(-1);

    if (type == ARC) {
        double angle1 = (pi / 180) * startAngle;
        double angle2 = (pi / 180) * (startAngle + spanAngle);
        x1 = x0 + lround(radius * cos(angle1));
        y1 = y0 - lround(radius * sin(angle1));
        x2 = x0 + lround(radius * cos(angle2));
        y2 = y0 - lround(radius * sin(angle2));
    }
}

bool Segment::insideCrossSegment(const Segment &s, int &x, int &y)
{
    if (type != LINE || s.type != LINE)
        return false;

    if (length() == 0 || s.length() == 0)
        return false;

    if ((x1 == s.x1 && y1 == s.y1) || (x1 == s.x2 && y1 == s.y2) ||
        (x2 == s.x1 && y2 == s.y1) || (x2 == s.x2 && y2 == s.y2))
        return false;

    Line line(x1, y1, x2, y2);
    Line line2(s.x1, s.y1, s.x2, s.y2);

    if (line.crossLine(line2, x, y)) {
        if ((x == x1 && y == y1) || (x == x2 && y == y2) ||
            (x == s.x1 && y == s.y1) || (x == s.x2 && y == s.y2))
            return false;
        return true;
    }

    return false;
}

bool Segment::isEndPointInsideSegment(const Segment &s, int &x, int &y)
{
    if (type != LINE || s.type != LINE)
        return false;

    if (length() == 0 || s.length() == 0)
        return false;

    if ((x1 == s.x1 && y1 == s.y1) || (x1 == s.x2 && y1 == s.y2) ||
        (x2 == s.x1 && y2 == s.y1) || (x2 == s.x2 && y2 == s.y2))
        return false;

    Line line(s.x1, s.y1, s.x2, s.y2);

    if (line.crossPoint(x1, y1)) {
        x = x1;
        y = y1;
        return true;
    }

    if (line.crossPoint(x2, y2)) {
        x = x2;
        y = y2;
        return true;
    }

    return false;
}

int Segment::length() const
{
    const double pi = acos(-1);
    double l = 0;

    if (type == LINE)
        l = hypot(x1 - x2, y1 - y2);

    if (type == ARC)
        l = fabs((pi / 180) * spanAngle * radius);

    return lround(l);
}

bool Segment::nearestPoint(int x_, int y_, int &x, int &y)
{
    if (type != LINE  || !length())
        return false;

    if (x1 != x2 && y1 != y2)
        return false;

    x = x_;
    y = y_;

    if (y1 == y2)
        limit(x, std::min(x1, x2), std::max(x1, x2));
    if (x1 == x2)
        limit(y, std::min(y1, y2), std::max(y1, y2));

    return true;
}

bool Segment::reduceLength(int x, int y, int delta)
{
    if (type != LINE)
        return false;

    bool isFirstEndPoint = x == x1 && y == y1;
    bool isSecondEndPoint = x == x2 && y == y2;

    if (!isFirstEndPoint && !isSecondEndPoint)
        return false;

    if (delta < 1 || delta > length())
        return false;

    int *px1 = isFirstEndPoint ? &x1 : &x2;
    int *py1 = isFirstEndPoint ? &y1 : &y2;
    int *px2 = isFirstEndPoint ? &x2 : &x1;
    int *py2 = isFirstEndPoint ? &y2 : &y1;

    if (x1 == x2) {
        if (*py1 < *py2)
            *py1 += delta;
        else
            *py1 -= delta;
        return true;
    }

    if (y1 == y2) {
        if (*px1 < *px2)
            *px1 += delta;
        else
            *px1 -= delta;
        return true;
    }

    if (abs(x1 - x2) == abs(y1 - y2)) {
        int delta2 = lround(delta / sqrt(2));
        *px1 += lround(copysign(1, *px2 - *px1)) * delta2;
        *py1 += lround(copysign(1, *py2 - *py1)) * delta2;
        return true;
    }

    return false;
}

bool Segment::set90DegreesTurnArc(int turn, int x, int y, int radius_,
                                  int net_, int width_)
{
    if (turn < 0 || turn > 7 || radius_ <= 0 || width_ <= 0)
        return false;

    clear();

    net = net_;
    radius = radius_;
    type = ARC;
    width = width_;

    const double pi = acos(-1);
    int arcCenterTurnAngle = 45 * turn;
    double a = (pi / 180) * arcCenterTurnAngle;

    x0 = lround(x + sqrt(2) * radius * cos(a));
    y0 = lround(y - sqrt(2) * radius * sin(a));

    startAngle = (arcCenterTurnAngle + 135) % 360;
    spanAngle = 90;

    init();

    return true;
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
