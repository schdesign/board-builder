// track.h
// Copyright (C) 2018 Alexander Karpeko

#include "track.h"

Segment::Segment(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x1 = object["x1"].toInt();
    y1 = object["y1"].toInt();
    x2 = object["x2"].toInt();
    y2 = object["y2"].toInt();
    net = object["net"].toInt();
    width = object["width"].toInt();
}

void Segment::fromJson(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x1 = object["x1"].toInt();
    y1 = object["y1"].toInt();
    x2 = object["x2"].toInt();
    y2 = object["y2"].toInt();
    net = object["net"].toInt();
    width = object["width"].toInt();
}

QJsonObject Segment::toJson()
{
    QJsonObject object
    {
        {"x1", x1},
        {"y1", y1},
        {"x2", x2},
        {"y2", y2},
        {"net", net},
        {"width", width}
    };

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
