// element.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "element.h"
#include "exceptiondata.h"
#include "function.h"
#include "text.h"
#include <cmath>
#include <QJsonArray>
#include <QJsonDocument>

enum ElementOrientation {UP, RIGHT, DOWN, LEFT};

std::vector<Package> Element::packages;

Element::Element(int refX, int refY, int orientation, QString name,
                 QString packageName, QString reference):
    orientation(orientation), refX(refX), refY(refY),
    name(name), packageName(packageName), reference(reference)
{
    for (uint i = 0; i < Element::packages.size(); i++) {
        if (!packageName.compare(Element::packages[i].name)) {
                packageID = i;
                break;
        }
        if (i == Element::packages.size() - 1)
            throw ExceptionData("Package name error: " + packageName);
    }

    init(packages[packageID]);
}

Element::Element(int refX, int refY, int orientation, QString name,
                 const Package &package, QString reference):
    orientation(orientation), refX(refX), refY(refY),
    name(name), packageName(package.name), reference(reference)
{
    init(package);
}

Element::Element(const QJsonObject &object)
{
    reference = object["reference"].toString();
    name = object["name"].toString();
    packageName = object["package"].toString();

    for (uint i = 0; i < Element::packages.size(); i++) {
        if (!packageName.compare(Element::packages[i].name)) {
                packageID = i;
                break;
        }
        if (i == Element::packages.size() - 1)
            throw ExceptionData("Package name error: " + packageName);
    }

    refX = object["refX"].toInt();
    refY = object["refY"].toInt();
    QString orientationString(object["orientation"].toString());

    for (int i = 0; i < 4; i++) {
        if (!orientationString.compare(elementOrientationString[i])) {
            orientation = i;
            break;
        }
        if (i == 3)
            throw ExceptionData("Element orientation error");
    }

    init(packages[packageID]);

    QJsonArray elementPads = object["pads"].toArray();

    for (auto e : elementPads) {
        QJsonObject elementPad = e.toObject();
        int number = elementPad["number"].toInt();
        if (number < 1)
            throw ExceptionData("Pad number error");
        if (pads[number-1].number != number)
            throw ExceptionData("Pad number error");
        pads[number-1].net = elementPad["net"].toInt();
    }
}

Element::Element(const QJsonObject &object, int refX, int refY):
    refX(refX), refY(refY)
{
    reference = object["reference"].toString();
    name = object["name"].toString();
    packageName = object["package"].toString();

    for (uint i = 0; i < Element::packages.size(); i++) {
        if (!packageName.compare(Element::packages[i].name)) {
                packageID = i;
                break;
        }
        if (i == Element::packages.size() - 1)
            throw ExceptionData("Package name error: " + packageName);
    }

    orientation = UP;

    init(packages[packageID]);

    QJsonArray elementPads = object["pads"].toArray();

    for (auto e : elementPads) {
        QJsonObject elementPad = e.toObject();
        int number = elementPad["number"].toInt();
        if (number < 1)
            throw ExceptionData("Pad number error");
        if (pads[number-1].number != number)
            throw ExceptionData("Pad number error");
        pads[number-1].net = elementPad["net"].toInt();
    }
}

void Element::addPackage(const QJsonValue &value)
{
    Package package(value);
    Element::packages.push_back(package);
}

void Element::draw(QPainter &painter, const Layers &layers, int fontSize, double scale)
{
    int align;
    int x, y, w, h, rx, ry;
    QString str;

    if (layers.draw & (1 << PAD_LAYER)) {
        painter.setPen(layers.color[PAD_LAYER]);
        for (auto p : pads) {
            w = scale * p.width;
            h = scale * p.height;
            if (p.orientation == RIGHT) {
                h = scale * p.width;
                w = scale * p.height;
            }
            x = scale * p.x - w / 2;
            y = scale * p.y - h / 2;
            rx = 0.5 * scale * p.diameter;
            ry = rx;
            QPainterPath path;
            path.addRoundedRect(x, y, w, h, rx, ry);
            painter.setBrush(layers.color[PAD_LAYER]);
            painter.drawPath(path);
            if (2 * rx == w && 2 * ry == h) {
                QPainterPath path2;
                path2.addRoundedRect(x + w / 4, y + h / 4, w / 2, h / 2, rx / 2, ry / 2);
                painter.setBrush(QColor(255, 255, 255));
                painter.drawPath(path2);
            }
            //painter.drawText(x, y, w, h, Qt::AlignCenter, str.setNum(p.net));
        }
    }

    if (layers.draw & (1 << PACKAGE_LAYER)) {
        painter.setPen(layers.color[PACKAGE_LAYER]);
        for (auto e : ellipses)
            painter.drawEllipse(scale * (e.x - e.w / 2),
                                scale * (e.y - e.h / 2),
                                scale * e.w, scale * e.h);
        for (auto l : lines)
            painter.drawLine(scale * l.x1, scale * l.y1,
                             scale * l.x2, scale * l.y2);
    }

    if (layers.draw & (1 << NAME_LAYER)) {
        painter.setPen(layers.color[NAME_LAYER]);
        w = fontSize * name.size();
        h = fontSize;
        x = scale * centerX - w / 2;
        y = scale * border.bottomY + 0.2 * fontSize;
        nameTextAlignH = "Center";
        nameTextAlignV = "Center";
        align = alignment(nameTextAlignH, nameTextAlignV);
        painter.drawText(x, y, w, h, align, name);
    }

    if (layers.draw & (1 << REFERENCE_LAYER)) {
        painter.setPen(layers.color[REFERENCE_LAYER]);
        w = fontSize * name.size();
        h = fontSize;
        x = scale * centerX - w / 2;
        y = scale * border.topY - 1.2 * fontSize;
        referenceTextAlignH = "Center";
        referenceTextAlignV = "Center";
        align = alignment(referenceTextAlignH, referenceTextAlignV);
        painter.drawText(x, y, w, h, align, reference);
    }
}

bool Element::exist(int x, int y)
{
    int dx = abs(border.rightX - border.leftX) / 2;
    int dy = abs(border.bottomY - border.topY) / 2;

    if (x >= (centerX - dx) && x <= (centerX + dx) &&
        y >= (centerY - dy) && y <= (centerY + dy))
        return true;

    return false;
}

void Element::init(const Package &package)
{
    if (orientation < 0 || orientation > 3)
        orientation = 0;

    const int c[4][4] = {{1,0,0,1}, {0,-1,1,0}, {-1,0,0,-1}, {0,1,-1,0}};
    int t = orientation;

    int leftX = refX + c[t][0] * package.border.leftX + c[t][1] * package.border.topY;
    int topY = refY + c[t][2] * package.border.leftX + c[t][3] * package.border.topY;
    int rightX = refX + c[t][0] * package.border.rightX + c[t][1] * package.border.bottomY;
    int bottomY = refY + c[t][2] * package.border.rightX + c[t][3] * package.border.bottomY;

    border.leftX = leftX < rightX ? leftX : rightX;
    border.topY = topY < bottomY ? topY : bottomY;
    border.rightX = leftX < rightX ? rightX : leftX;
    border.bottomY = topY < bottomY ? bottomY : topY;

    centerX = (border.leftX + border.rightX) / 2;
    centerY = (border.topY + border.bottomY) / 2;

    nameTextAlignH = "";
    nameTextAlignV = "";
    nameTextHeight = 0;
    nameTextX = 0;
    nameTextY = 0;

    referenceTextAlignH = "";
    referenceTextAlignV = "";
    referenceTextHeight = 0;
    referenceTextX = 0;
    referenceTextY = 0;

    Ellipse ellipse;
    for (auto e : package.ellipses) {
        ellipse.x = refX + c[t][0] * e.x + c[t][1] * e.y;
        ellipse.y = refY + c[t][2] * e.x + c[t][3] * e.y;
        ellipse.w = e.w;
        ellipse.h = e.h;
        ellipses.push_back(ellipse);
    }

    Line line;
    for (auto p : package.lines) {
        line.x1 = refX + c[t][0] * p.x1 + c[t][1] * p.y1;
        line.y1 = refY + c[t][2] * p.x1 + c[t][3] * p.y1;
        line.x2 = refX + c[t][0] * p.x2 + c[t][1] * p.y2;
        line.y2 = refY + c[t][2] * p.x2 + c[t][3] * p.y2;
        lines.push_back(line);
    }

    Pad pad;
    for (auto p : package.pads) {
        pad.diameter = p.diameter;
        pad.height = p.height;
        pad.innerDiameter = p.innerDiameter;
        pad.number = p.number;
        pad.orientation = p.orientation;
        if (orientation == LEFT || orientation == RIGHT) {
            if (pad.orientation == UP)
                pad.orientation = RIGHT;
            else if (pad.orientation == RIGHT)
                pad.orientation = UP;
        }
        pad.typeNumber = p.typeNumber;
        pad.width = p.width;
        pad.x = refX + c[t][0] * p.x + c[t][1] * p.y;
        pad.y = refY + c[t][2] * p.x + c[t][3] * p.y;
        pads.push_back(pad);
    }
}

bool Element::inside(int leftX, int topY, int rightX, int bottomY)
{
    if (centerX >= leftX && centerX <= rightX &&
        centerY >= topY && centerY <= bottomY)
        return true;

    return false;
}

QJsonObject Element::toJson()
{
    QString orientationString(elementOrientationString[orientation]);
    QJsonArray elementPads;

    for (uint i = 0; i < pads.size(); i++) {
        QJsonObject elementPad
        {
            {"number", int(i + 1)},
            {"net", pads[i].net}
        };

        elementPads.append(elementPad);
    }

    QJsonObject object
    {
        {"reference", reference},
        {"name", name},
        {"package", packageName},
        {"refX", refX},
        {"refY", refY},
        {"orientation", orientationString},
        {"pads", elementPads}
    };

    return object;
}

QJsonObject Element::writePackages(const QString &packageType)
{
    QJsonArray packageArray;
    for (auto e : Element::packages)
        if (!e.type.compare(packageType))
            packageArray.append(e.toJson());

    QJsonObject object
    {
        {"packages", packageArray},
        {"object", "packages"}
    };

    return object;
}
