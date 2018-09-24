// element.h
// Copyright (C) 2018 Alexander Karpeko
// Coordinate unit: 1 micrometer

#ifndef ELEMENT_H
#define ELEMENT_H

#include "layers.h"
#include "types.h"
#include <QJsonObject>
#include <QPainter>
#include <QString>
#include <QStringRef>
#include <vector>

const QString elementOrientationString[4] =
{
    "Up", "Right", "Down", "Left"
};

const QString textAlignmentString[1] =
{
    "Center"
};

// Rounded rectangle
class Pad
{
public:
    QJsonObject toJson();

    int number;         // number in package
    int net;            // net number
    int width;          // width > height
    int height;
    int radius;
    int orientation;    // up, right
    int x;              // center
    int y;
};

class Package
{
public:
    QJsonObject toJson();

    Border border;
    Ellipse ellipse;
    int nameTextHeight;
    int nameTextAlignmentX;
    int nameTextAlignmentY;
    int nameTextX[4];       // left bottom point of text
    int nameTextY[4];
    int referenceTextHeight;
    int referenceTextAlignmentX;
    int referenceTextAlignmentY;
    int referenceTextX[4];  // up, right, down, left
    int referenceTextY[4];
    int refX;
    int refY;
    QString name;
    QString type;
    std::vector<Line> lines;
    std::vector<Pad> pads;
};

class Element
{
public:
    Element() {}
    Element(int refX, int refY, int orientation, QString name,
            QString packageName, QString reference);
    Element(const QJsonObject &object);
    Element(const QJsonObject &object, int refX, int refY);
    static void addPackage(const QJsonValue &value);
    static QJsonObject writePackages(const QString &packageType);
    void draw(QPainter &painter, const Layers &layers, double scale);
    bool exist(int x, int y);
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY);
    QJsonObject toJson();

    static std::vector<Package> packages;
    bool enabled;
    bool fixed;             // fixed on board
    bool group;
    Border border;
    Ellipse ellipse;        // point to pad 1
    int centerX;
    int centerY;
    int nameID;
    int nameTextX;          // left bottom point of text
    int nameTextY;
    int orientation;        // up, right, down, left
    int packageID;
    int referenceTextX;     // left bottom point of text
    int referenceTextY;
    int refX;               // point of pad 1
    int refY;
    QString name;           // name or value
    QString packageName;    // SMD0805
    QString reference;      // R1
    std::vector<Line> lines;
    std::vector<Pad> pads;
};

#endif  // ELEMENT_H
