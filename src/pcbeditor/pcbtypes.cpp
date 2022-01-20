// pcbtypes.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "layers.h"
#include "pcbtypes.h"
#include <cmath>
#include <QJsonArray>
#include <QPainterPath>

Polygon::Polygon(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    fill = object["fill"].toBool();
    net = object["net"].toInt();

    QJsonArray pointArray = object["points"].toArray();

    for (auto p : pointArray) {
        Point point(p);
        points.push_back(point);
    }
}

bool Polygon::center(int &x, int &y)
{
    if (points.size() < 3)
        return false;

    double x2 = 0;
    double y2 = 0;

    for (auto p : points) {
        x2 += p.x;
        y2 += p.y;
    }

    x = lround(x2 / points.size());
    y = lround(y2 / points.size());

    return true;
}

void Polygon::draw(QPainter &painter, double scale, QBrush brush)
{
    QPainterPath path;

    if (points.size() > 2) {
        path.moveTo(scale * points[0].x, scale * points[0].y);
        for (uint i = 1; i < points.size(); i++)
            path.lineTo(scale * points[i].x, scale * points[i].y);
        path.lineTo(scale * points[0].x, scale * points[0].y);
        painter.drawPath(path);
        if (fill)
            painter.fillPath(path, brush);
    }
}

void Polygon::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    fill = object["fill"].toBool();
    net = object["net"].toInt();

    QJsonArray pointArray = object["points"].toArray();

    for (auto p : pointArray) {
        Point point(p);
        points.push_back(point);
    }
}

bool Polygon::hasInnerPoint(int x, int y)
{
    int b = 0;

    for (auto p : points) {
        if (p.x < x && p.y < y)
            b |= 1;
        if (p.x < x && p.y > y)
            b |= 2;
        if (p.x > x && p.y < y)
            b |= 4;
        if (p.x > x && p.y > y)
            b |= 8;
    }
    if (b == 15)
        return true;

    return false;
}

QJsonObject Polygon::toJson()
{
    QJsonArray pointArray;
    for (auto p : points)
        pointArray.append(p.toJson());

    QJsonObject object
    {
        {"fill", fill},
        {"net", net},
        {"points", pointArray}
    };

    return object;
}

Via::Via(int x, int y):
    x(x), y(y)
{
    diameter = 0;
    innerDiameter = 0;
    net = -1;
}

Via::Via(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    diameter = object["diameter"].toInt();
    innerDiameter = object["innerDiameter"].toInt();
    net = object["net"].toInt();
    x = object["x"].toInt();
    y = object["y"].toInt();
}

void Via::draw(QPainter &painter, int layerNumber, double scale, int space)
{
    if (layerNumber != TOP_VIA_LAYER && layerNumber != BOTTOM_VIA_LAYER)
        return;

    int r = lround(scale * (diameter / 2 + space));
    int inR = lround(scale * innerDiameter / 2);
    int x2 = lround(scale * x) - r;
    int y2 = lround(scale * y) - r;

    QColor color(layerColors[layerNumber][0], layerColors[layerNumber][1],
                 layerColors[layerNumber][2], 255);

    if (space == 0) {
        painter.setPen(color);
        painter.setBrush(color);
    }
    else {
        painter.setPen(Qt::white);
        painter.setBrush(Qt::white);
    }

    QPainterPath path;
    path.addRoundedRect(x2, y2, 2 * r, 2 * r, r, r);
    painter.drawPath(path);

    if (space == 0) {
        painter.setPen(Qt::white);
        painter.setBrush(Qt::white);
        QPainterPath path2;
        path2.addRoundedRect(x2 + r - inR, y2 + r - inR, 2 * inR, 2 * inR, inR, inR);
        painter.drawPath(path2);
    }
}

bool Via::exist(int x_, int y_)
{
    int r = diameter / 2;

    if (r > 0)
        if (lround(hypot((x_ - x), (y_ - y))) < r)
            return true;

    return false;
}

void Via::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    diameter = object["diameter"].toInt();
    innerDiameter = object["innerDiameter"].toInt();
    net = object["net"].toInt();
    x = object["x"].toInt();
    y = object["y"].toInt();
}

QJsonObject Via::toJson()
{
    QJsonObject object
    {
        {"diameter", diameter},
        {"innerDiameter", innerDiameter},
        {"net", net},
        {"x", x},
        {"y", y}
    };

    return object;
}

// Horizontal and vertical lines
bool crossLines2(const double line1[], const double line2[], double &x, double &y)
{
    double x11 = line1[0];
    double y11 = line1[1];
    double x12 = line1[2];
    double y12 = line1[3];
    double x21 = line2[0];
    double y21 = line2[1];
    double x22 = line2[2];
    double y22 = line2[3];

    if ((fabs(x11 - x12) < minValue) && (fabs(y21 - y22) < minValue)) {
        if (((x11 > (x21 - minValue) && x11 < (x22 + minValue)) ||
             (x11 > (x22 - minValue) && x11 < (x21 + minValue))) &&
            ((y21 > (y11 - minValue) && y21 < (y12 + minValue)) ||
             (y21 > (y12 - minValue) && y21 < (y11 + minValue)))) {
            x = x11;
            y = y21;
            return true;
        }
    }

    if ((fabs(y11 - y12) < minValue) && (fabs(x21 - x22) < minValue)) {
        if (((y11 > (y21 - minValue) && y11 < (y22 + minValue)) ||
             (y11 > (y22 - minValue) && y11 < (y21 + minValue))) &&
            ((x21 > (x11 - minValue) && x21 < (x12 + minValue)) ||
             (x21 > (x12 - minValue) && x21 < (x11 + minValue)))) {
            x = x21;
            y = y11;
            return true;
        }
    }

    return false;
}

bool crossPoint2(const double line[], double x, double y)
{
    double x1 = line[0];
    double y1 = line[1];
    double x2 = line[2];
    double y2 = line[3];

    // Horizontal line
    if (fabs(y1 - y2) < minValue) {
        if ((fabs(y - y1) < minValue) &&
            ((x > (x1 - minValue) && x < (x2 + minValue)) ||
             (x > (x2 - minValue) && x < (x1 + minValue))))
            return true;
        else
            return false;
    }

    // Vertical line
    if (fabs(x1 - x2) < minValue) {
        if ((fabs(x - x1) < minValue) &&
            ((y > (y1 - minValue) && y < (y2 + minValue)) ||
             (y > (y2 - minValue) && y < (y1 + minValue))))
            return true;
        else
            return false;
    }

    return false;
}

bool joinLines2(double line1[], const double line2[])
{
    double min1, max1, min2, max2;

    // Horizontal line
    if (fabs(line1[1] - line1[3]) < minValue &&
        fabs(line2[1] - line2[3]) < minValue &&
        fabs(line1[1] - line2[1]) < minValue) {
        min1 = line1[0];
        max1 = line1[2];
        if (line1[2] < line1[0]) {
            min1 = line1[2];
            max1 = line1[0];
        }
        min2 = line2[0];
        max2 = line2[2];
        if (line2[2] < line2[0]) {
            min2 = line2[2];
            max2 = line2[0];
        }
        if (max1 > min2 - minValue && max2 > min1 - minValue) {
            line1[0] = min1;
            if (min2 < min1)
                line1[0] = min2;
            line1[2] = max1;
            if (max2 > max1)
                line1[2] = max2;
            return true;
        }
    }

    // Vertical line
    if (fabs(line1[0] - line1[2]) < minValue &&
        fabs(line2[0] - line2[2]) < minValue &&
        fabs(line1[0] - line2[0]) < minValue) {
        min1 = line1[1];
        max1 = line1[3];
        if (line1[3] < line1[1]) {
            min1 = line1[3];
            max1 = line1[1];
        }
        min2 = line2[1];
        max2 = line2[3];
        if (line2[3] < line2[1]) {
            min2 = line2[3];
            max2 = line2[1];
        }
        if (max1 > min2 - minValue && max2 > min1 - minValue) {
            line1[1] = min1;
            if (min2 < min1)
                line1[1] = min2;
            line1[3] = max1;
            if (max2 > max1)
                line1[3] = max2;
            return true;
        }
    }

    return false;
}
