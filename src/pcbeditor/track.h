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
    enum TYPE {LINE, ARC};

    Segment();
    Segment(int x1, int y1, int x2, int y2, int net, int width);
    Segment(int x0, int y0, int radius, int startAngle,
            int spanAngle, int net, int width);
    Segment(const QJsonValue &value);
    void clear();
    bool crossPoint(int x, int y);
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int net;         // net number
    int radius;
    int spanAngle;   // angle = 0 degree: 3 o'clock position
    int startAngle;  // angle > 0 degree: counter-clockwise direction
    int type;
    int width;       // um
    int x0;          // circle center
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
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
