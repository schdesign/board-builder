// element.h
// Copyright (C) 2018 Alexander Karpeko
// Coordinate unit: 1 micrometer

#ifndef ELEMENT_H
#define ELEMENT_H

#include "layers.h"
#include "package.h"
#include <QPainter>
#include <QStringRef>

const QString elementOrientationString[4] =
{
    "Up", "Right", "Down", "Left"
};

const QString textAlignmentString[1] =
{
    "Center"
};

class Element
{
public:
    Element() {}
    Element(int refX, int refY, int orientation, QString name,
            QString packageName, QString reference);
    Element(int refX, int refY, int orientation, QString name,
            const Package &package, QString reference);
    Element(const QJsonObject &object);
    Element(const QJsonObject &object, int refX, int refY);
    static void addPackage(const QJsonValue &value);
    static QJsonObject writePackages(const QString &packageType);
    void draw(QPainter &painter, const Layers &layers, double scale);
    bool exist(int x, int y);
    void init(const Package &package);
    bool inside(int leftX, int topY, int rightX, int bottomY);
    QJsonObject toJson();

    static std::vector<Package> packages;
    bool enabled;
    bool fixed;             // fixed on board
    bool group;
    Border border;
    int centerX;
    int centerY;
    int nameID;
    int nameTextHeight;
    int nameTextX;
    int nameTextY;
    int orientation;        // up, right, down, left
    int packageID;
    int referenceTextHeight;
    int referenceTextX;
    int referenceTextY;
    int refX;               // point of pad 1
    int refY;
    QString name;           // name or value
    QString nameTextAlignH;
    QString nameTextAlignV;
    QString packageName;    // SMD0805
    QString reference;      // R1
    QString referenceTextAlignH;
    QString referenceTextAlignV;
    std::vector<Ellipse> ellipses;
    std::vector<Line> lines;
    std::vector<Pad> pads;
};

#endif  // ELEMENT_H
