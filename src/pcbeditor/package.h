// package.h
// Copyright (C) 2019 Alexander Karpeko

#ifndef PACKAGE_H
#define PACKAGE_H

#include "types.h"
#include <QString>
#include <vector>

class Pad
{
public:
    QJsonObject toJson();

    int diameter;
    int height;
    int innerDiameter;
    int net;            // net number
    int number;         // number in package
    int orientation;    // up, right
    int typeNumber;     // 0, 1, 2
    int width;
    int x;              // center
    int y;
};

class PadTypeParams
{
public:
    QJsonObject toJson();

    int diameter;
    int height;
    int innerDiameter;
    int width;
};

class Package
{
public:
    Package() {}
    Package(const QJsonValue &value);
    void clear();
    QJsonObject toJson();

    Border border;
    QString name;
    QString type;       // SMD, DIP
    std::vector<Ellipse> ellipses;
    std::vector<Line> lines;
    std::vector<Pad> pads;
    std::vector<PadTypeParams> padTypesParams;
};

#endif  // PACKAGE_H
