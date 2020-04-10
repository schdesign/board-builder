// element.h
// Copyright (C) 2018 Alexander Karpeko
// Coordinate unit: 1 micrometer

#ifndef ELEMENT_H
#define ELEMENT_H

#include "layers.h"
#include "package.h"
#include <QPainter>
#include <QStringRef>

class ElementDrawingOptions
{
public:
    bool fillPads;
    double scale;
    int fontSize;
};

class Element
{
public:
    constexpr static double minPadCornerRadius = 0.001;
    constexpr static double maxPadCornerRadius = 1000000;

private:
    const char *elementOrientationString[4] =
    {
        "Up", "Right", "Down", "Left"
    };

    const char *textAlignmentString[1] =
    {
        "Center"
    };

public:
    enum ElementOrientation {UP, RIGHT, DOWN, LEFT};

    Element() {}
    Element(int refX, int refY, int orientation, const QString &name,
            const QString &packageName, const QString &reference, bool hasOptions = true);
    Element(int refX, int refY, int orientation, const QString &name,
            const Package &package, const QString &reference, bool hasOptions = true);
    Element(const QJsonObject &object, bool hasOptions = true);
    Element(const QJsonObject &object, int refX, int refY, bool hasOptions = true);
    static void addPackage(const QJsonValue &value);
    static QJsonObject writePackages(const QString &packageType);
    void draw(QPainter &painter, const Layers &layers,
              const ElementDrawingOptions &options);
    bool exist(int x, int y);
    void findOuterBorder();
    void init(const Package &package);
    bool inside(int leftX, int topY, int rightX, int bottomY);
    void roundPadCorners();
    QJsonObject toJson();

    static double padCornerRadius;
    static std::vector<Package> packages;
    bool enabled;
    bool fixed;             // fixed on board
    bool group;
    int centerX;            // border center
    int centerY;
    int nameID;
    int nameTextHeight;
    int nameTextX;
    int nameTextY;
    int orientation;        // up, right, down, left
    int outerBorderCenterX;
    int outerBorderCenterY;
    int packageID;
    int referenceTextHeight;
    int referenceTextX;
    int referenceTextY;
    int refX;               // point of pad 1
    int refY;
    Border border;
    Border outerBorder;
    QString name;           // name or value
    QString nameTextAlignH;
    QString nameTextAlignV;
    QString packageName;    // SMD0805
    QString reference;      // R1
    QString referenceTextAlignH;
    QString referenceTextAlignV;
    QString type;           // SMD, DIP
    std::vector<Ellipse> ellipses;
    std::vector<Line> lines;
    std::vector<Pad> pads;
};

#endif  // ELEMENT_H
