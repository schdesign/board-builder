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
    enum TYPE {LINE, ARC};

public:
    Segment();
    Segment(int x1, int y1, int x2, int y2, int net, int width);
    Segment(int x, int y, int radius, int startAngle,
            int spanAngle, int net, int width);
    Segment(const QJsonValue &value);
    void clear();
    void fromJson(const QJsonValue &value);
    QJsonObject toJson();

    int net;    // net number
    int radius;
    int spanAngle;
    int startAngle;
    int type;
    int width;  // um
    int x;      // circle center
    int y;
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
