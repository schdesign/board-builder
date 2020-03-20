// track.h
// Copyright (C) 2018 Alexander Karpeko

#include "track.h"

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
    x = 0;
    y = 0;
}

Segment::Segment(int x, int y, int radius, int startAngle,
                 int spanAngle, int net, int width):
    net(net), radius(radius), spanAngle(spanAngle),
    startAngle(startAngle), type(ARC), width(width), x(x), y(y)
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
    x = 0;
    y = 0;
    x1 = 0;
    y1 = 0;
    x2 = 0;
    y2 = 0;
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
        x = object["x"].toInt();
        y = object["y"].toInt();
    }

    net = object["net"].toInt();
    width = object["width"].toInt();
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
            {"x", x},
            {"y", y}
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
