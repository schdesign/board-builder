// track.h
// Copyright (C) 2018 Alexander Karpeko
// Track connect all pads of net.
// Curve is part of track.
// Curve connect 2 pads or pad and other curve.
// Segment is part of curve.
// Segment is cubic bezier curve.
// Coordinate unit: 1 micrometer

#ifndef TRACK_H
#define TRACK_H

#include <QJsonObject>
#include <QJsonValue>

class Segment
{
public:
    Segment() {}
    Segment(int x1, int y1, int x2, int y2, int net, int width):
         x1(x1), y1(y1), x2(x2), y2(y2), net(net), width(width) {}
    Segment(const QJsonValue &value);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int x1;
    int y1;
    int x2;
    int y2;
    int net;    // net number
    int width;  // um
};

/*
#include <QPainter>
#include <vector>

class Segment
{
public:
    double x[4];
    double y[4];
};

typedef std::vector<Segment> Curve;

class Track
{
public:
    double width;
    int net;
    std::vector <Curve> curves;

    void addSegment(int x1, int y1, int x2, int y2);
    void clear();
    void deleteSegment(int x1, int y1, int x2, int y2);
    void draw(QPainter &painter);
    void improve();
    void improveCurve(Curve &curve);
};
*/

#endif  // TRACK_H
