// types.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "types.h"
#include <cmath>

Arc::Arc(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
    w = object["w"].toInt();
    h = object["h"].toInt();
    startAngle = object["startAngle"].toInt();
    spanAngle = object["spanAngle"].toInt();
}

void Arc::clear()
{
    x = 0;
    y = 0;
    w = 0;
    h = 0;
    startAngle = 0;
    spanAngle = 0;
}

void Arc::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
    w = object["w"].toInt();
    h = object["h"].toInt();
    startAngle = object["startAngle"].toInt();
    spanAngle = object["spanAngle"].toInt();
}

QJsonObject Arc::toJson()
{
    QJsonObject object
    {
        {"x", x},
        {"y", y},
        {"w", w},
        {"h", h},
        {"startAngle", startAngle},
        {"spanAngle", spanAngle}
    };

    return object;
}

Border::Border(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    leftX = object["leftX"].toInt();
    topY = object["topY"].toInt();
    rightX = object["rightX"].toInt();
    bottomY = object["bottomY"].toInt();
}

void Border::clear()
{
    leftX = 0;
    topY = 0;
    rightX = 0;
    bottomY = 0;
}

void Border::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    leftX = object["leftX"].toInt();
    topY = object["topY"].toInt();
    rightX = object["rightX"].toInt();
    bottomY = object["bottomY"].toInt();
}

QJsonObject Border::toJson()
{
    QJsonObject object
    {
        {"leftX", leftX},
        {"topY", topY},
        {"rightX", rightX},
        {"bottomY", bottomY}
    };

    return object;
}

Ellipse::Ellipse(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
    w = object["w"].toInt();
    h = object["h"].toInt();
}

void Ellipse::clear()
{
    x = 0;
    y = 0;
    w = 0;
    h = 0;
}

void Ellipse::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
    w = object["w"].toInt();
    h = object["h"].toInt();
}

QJsonObject Ellipse::toJson()
{
    QJsonObject object
    {
        {"x", x},
        {"y", y},
        {"w", w},
        {"h", h}
    };

    return object;
}

Line::Line(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x1 = object["x1"].toInt();
    y1 = object["y1"].toInt();
    x2 = object["x2"].toInt();
    y2 = object["y2"].toInt();
}

void Line::clear()
{
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
}

bool Line::crossLine(const Line &line, int &x, int &y)
{
    if (length() == 0 || line.length() == 0)
        return false;

    double a1, a2, c1, c2;

    if (x1 == x2 && line.x1 == line.x2)
        return false;

    if (x1 != x2) {
        a1 = (y2 - y1) / (x1 - x2);
        c1 = -a1 * x1 - y1;
    }

    if (line.x1 != line.x2) {
        a2 = (line.y2 - line.y1) / (line.x1 - line.x2);
        c2 = -a2 * line.x1 - line.y1;
    }

    if (x1 == x2) {
        x = x1;
        y = lround(-a2 * x - c2);
        return crossPoint(x, y) && line.crossPoint(x, y);
    }

    if (line.x1 == line.x2) {
        x = line.x1;
        y = lround(-a1 * x - c1);
        return crossPoint(x, y) && line.crossPoint(x, y);
    }

    x = lround((c2 - c1) / (a1 - a2));
    y = lround(-a1 * x - c1);
    return crossPoint(x, y) && line.crossPoint(x, y);
}

bool Line::crossPoint(int x, int y) const
{
    if (x1 == x2) {
        if ((x == x1) && ((y >= y1 && y <= y2) || (y >= y2 && y <= y1)))
            return true;
        else
            return false;
    }

    double k = (y2 - y1) / (x2 - x1);
    double a = y1 - k * x1;

    if ((fabs(y - k * x - a) < minValue) &&
        ((x >= x1 && x <= x2) || (x >= x2 && x <= x1)) &&
        ((y >= y1 && y <= y2) || (y >= y2 && y <= y1)))
        return true;

    return false;
}

void Line::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x1 = object["x1"].toInt();
    y1 = object["y1"].toInt();
    x2 = object["x2"].toInt();
    y2 = object["y2"].toInt();
}

// Horizontal or vertical lines
bool Line::join(const Line &line)
{
    int min1, max1, min2, max2;

    if (x1 == x2 && x1 == line.x1 && line.x1 == line.x2) {
        min1 = y1;
        max1 = y2;
        if (y2 < y1) {
            min1 = y2;
            max1 = y1;
        }
        min2 = line.y1;
        max2 = line.y2;
        if (line.y2 < line.y1) {
            min2 = line.y2;
            max2 = line.y1;
        }
        if (max1 >= min2 && max2 >= min1) {
            y1 = min1;
            if (min2 < min1)
                y1 = min2;
            y2 = max1;
            if (max2 > max1)
                y2 = max2;
            return true;
        }
    }

    if (y1 == y2 && y1 == line.y1 && line.y1 == line.y2) {
        min1 = x1;
        max1 = x2;
        if (x2 < x1) {
            min1 = x2;
            max1 = x1;
        }
        min2 = line.x1;
        max2 = line.x2;
        if (line.x2 < line.x1) {
            min2 = line.x2;
            max2 = line.x1;
        }
        if (max1 >= min2 && max2 >= min1) {
            x1 = min1;
            if (min2 < min1)
                x1 = min2;
            x2 = max1;
            if (max2 > max1)
                x2 = max2;
            return true;
        }
    }

    return false;
}

int Line::length() const
{
    double l = hypot(x1 - x2, y1 - y2);

    return lround(l);
}

QJsonObject Line::toJson()
{
    QJsonObject object
    {
        {"x1", x1},
        {"y1", y1},
        {"x2", x2},
        {"y2", y2}
    };

    return object;
}

Point::Point(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
}

void Point::clear()
{
    x = 0;
    y = 0;
}

void Point::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
}

QJsonObject Point::toJson()
{
    QJsonObject object
    {
        {"x", x},
        {"y", y}
    };

    return object;
}

bool Point::operator <(const Point &point) const
{
    if (x < point.x || (x == point.x && y < point.y))
        return true;
    return false;
}

bool Point::operator ==(const Point &point) const
{
    if (x == point.x && y == point.y)
        return true;
    return false;
}
