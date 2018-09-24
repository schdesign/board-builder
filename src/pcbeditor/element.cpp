// element.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "element.h"
#include "exceptiondata.h"
#include "text.h"
#include <cmath>
#include <QJsonArray>
#include <QJsonDocument>

enum ElementOrientation {UP, RIGHT, DOWN, LEFT};

std::vector<Package> Element::packages;

QJsonObject Pad::toJson()
{
    QJsonObject object
    {
        {"orientation", orientation},
        {"x", x},
        {"y", y}
    };

    return object;
}

QJsonObject Package::toJson()
{
    QJsonObject nameText
    {
        {"height", nameTextHeight},
        {"alignmentX", nameTextAlignmentX},
        {"alignmentY", nameTextAlignmentY},
        {"upX", nameTextX[0]},
        {"upY", nameTextY[0]},
        {"rightX", nameTextX[1]},
        {"rightY", nameTextY[1]},
        {"downX", nameTextX[2]},
        {"downY", nameTextY[2]},
        {"leftX", nameTextX[3]},
        {"leftY", nameTextY[3]}
    };

    QJsonObject referenceText
    {
        {"height", referenceTextHeight},
        {"alignmentX", referenceTextAlignmentX},
        {"alignmentY", referenceTextAlignmentY},
        {"upX", referenceTextX[0]},
        {"upY", referenceTextY[0]},
        {"rightX", referenceTextX[1]},
        {"rightY", referenceTextY[1]},
        {"downX", referenceTextX[2]},
        {"downY", referenceTextY[2]},
        {"leftX", referenceTextX[3]},
        {"leftY", referenceTextY[3]}
    };

    QJsonObject padParams
    {
        {"width", pads[0].width},
        {"height", pads[0].height},
        {"radius", pads[0].radius}
    };

    QJsonArray linesArray;
    for (auto l : lines)
        linesArray.append(l.toJson());

    QJsonArray padsArray;
    for (auto p : pads)
        padsArray.append(p.toJson());

    QJsonObject object
    {
        {"border", border.toJson()},
        {"ellipse", border.toJson()},
        {"name", name},
        {"nameText", nameText},
        {"referenceText", referenceText},
        {"type", type},
        {"padParams", padParams},
        {"lines", linesArray},
        {"pads", padsArray}
   };

    return object;
}

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

    init();
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

    init();

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

    init();

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
    Package package;

    package.refX = 0;
    package.refY = 0;

    QJsonObject object = value.toObject();

    package.name = object["name"].toString();
    package.type = object["type"].toString();

    QJsonObject padParamsObject = object["padParams"].toObject();
    QJsonArray padsArray = object["pads"].toArray();
    QJsonArray linesArray = object["lines"].toArray();
    QJsonObject referenceTextObject = object["referenceText"].toObject();
    QJsonObject nameTextObject = object["nameText"].toObject();

    int width = padParamsObject["width"].toInt();
    int height = padParamsObject["height"].toInt();
    int radius = padParamsObject["radius"].toInt();

    for (int i = 0; i < padsArray.size(); i++) {
        QJsonObject packagePad = padsArray[i].toObject();
        Pad pad;
        pad.number = i + 1;
        pad.net = 0;
        pad.width = width;
        pad.height = height;
        pad.radius = radius;
        pad.orientation = packagePad["orientation"].toInt();
        pad.x = packagePad["x"].toInt();
        pad.y = packagePad["y"].toInt();
        package.pads.push_back(pad);
    }

    package.ellipse.fromJson(object["ellipse"]);

    for (auto l : linesArray) {
        Line line(l);
        package.lines.push_back(line);
    }

    package.border.fromJson(object["border"]);

    package.referenceTextHeight = referenceTextObject["height"].toInt();
    package.referenceTextAlignmentX = referenceTextObject["alignmentX"].toInt();
    package.referenceTextAlignmentY = referenceTextObject["alignmentY"].toInt();
    package.referenceTextX[0] = referenceTextObject["upX"].toInt();
    package.referenceTextY[0] = referenceTextObject["upY"].toInt();
    package.referenceTextX[1] = referenceTextObject["rightX"].toInt();
    package.referenceTextY[1] = referenceTextObject["rightY"].toInt();
    package.referenceTextX[2] = referenceTextObject["downX"].toInt();
    package.referenceTextY[2] = referenceTextObject["downY"].toInt();
    package.referenceTextX[3] = referenceTextObject["leftX"].toInt();
    package.referenceTextY[3] = referenceTextObject["leftY"].toInt();

    package.nameTextHeight = nameTextObject["height"].toInt();
    package.nameTextAlignmentX = nameTextObject["alignmentX"].toInt();
    package.nameTextAlignmentY = nameTextObject["alignmentY"].toInt();
    package.nameTextX[0] = nameTextObject["upX"].toInt();
    package.nameTextY[0] = nameTextObject["upY"].toInt();
    package.nameTextX[1] = nameTextObject["rightX"].toInt();
    package.nameTextY[1] = nameTextObject["rightY"].toInt();
    package.nameTextX[2] = nameTextObject["downX"].toInt();
    package.nameTextY[2] = nameTextObject["downY"].toInt();
    package.nameTextX[3] = nameTextObject["leftX"].toInt();
    package.nameTextY[3] = nameTextObject["leftY"].toInt();

    Element::packages.push_back(package);
}

void Element::draw(QPainter &painter, const Layers &layers, double scale)
{
    int x, y, w, h, rx, ry;
    QString str;

    if (layers.draw & (1 << PAD)) {
        painter.setPen(layers.color[PAD]);
        for (auto p : pads) {
            w = scale * p.width;
            h = scale * p.height;
            if (p.orientation == RIGHT) {
                h = scale * p.width;
                w = scale * p.height;
            }
            x = scale * p.x - w / 2;
            y = scale * p.y - h / 2;
            rx = scale * p.radius;
            ry = rx;
            painter.drawRoundedRect(x, y, w, h, rx, ry);
            //painter.drawText(x, y, w, h, Qt::AlignCenter, str.setNum(p.net));
        }
    }

    if (layers.draw & (1 << PACKAGE)) {
        painter.setPen(layers.color[PACKAGE]);
        painter.drawEllipse(scale * (ellipse.x - ellipse.w / 2),
                            scale * (ellipse.y - ellipse.h / 2),
                            scale * ellipse.w, scale * ellipse.h);
        for (auto l : lines)
            painter.drawLine(scale * l.x1, scale * l.y1,
                             scale * l.x2, scale * l.y2);
    }

    if (layers.draw & (1 << NAME)) {
        painter.setPen(layers.color[NAME]);
        w = 3 * scale * (border.rightX - border.leftX);
        h = 5 * scale * packages[packageID].nameTextHeight;
        x = scale * nameTextX - w / 2;
        y = scale * nameTextY - h / 2;
        painter.drawText(x, y, w, h, Qt::AlignCenter, name);
    }

    if (layers.draw & (1 << REFERENCE)) {
        painter.setPen(layers.color[REFERENCE]);
        w = 3 * scale * (border.rightX - border.leftX);
        h = 5 * scale * packages[packageID].referenceTextHeight;
        x = scale * referenceTextX - w / 2;
        y = scale * referenceTextY - h / 2;
        painter.drawText(x, y, w, h, Qt::AlignCenter, reference);
    }
}

bool Element::exist(int x, int y)
{
    int dx = fabs(border.rightX - border.leftX) / 2;
    int dy = fabs(border.bottomY - border.topY) / 2;

    if (x >= (centerX - dx) && x <= (centerX + dx) &&
        y >= (centerY - dy) && y <= (centerY + dy))
        return true;

    return false;
}

void Element::init()
{
    const Package &package = packages[packageID];

    if (orientation < 0 || orientation > 3)
        orientation = 0;

    const int c[4][4] = {{1,0,0,1}, {0,-1,1,0}, {-1,0,0,-1}, {0,1,-1,0}};
    int t = orientation;

    border.leftX = refX + c[t][0] * package.border.leftX + c[t][1] * package.border.topY;
    border.topY = refY + c[t][2] * package.border.leftX + c[t][3] * package.border.topY;
    border.rightX = refX + c[t][0] * package.border.rightX + c[t][1] * package.border.bottomY;
    border.bottomY = refY + c[t][2] * package.border.rightX + c[t][3] * package.border.bottomY;

    centerX = (border.leftX + border.rightX) / 2;
    centerY = (border.topY + border.bottomY) / 2;

    referenceTextX = refX + package.referenceTextX[t];
    referenceTextY = refY + package.referenceTextY[t];

    nameTextX = refX + package.nameTextX[t];
    nameTextY = refY + package.nameTextY[t];

    ellipse.x = refX + c[t][0] * package.ellipse.x + c[t][1] * package.ellipse.y;
    ellipse.y = refY + c[t][2] * package.ellipse.x + c[t][3] * package.ellipse.y;
    ellipse.w = package.ellipse.w;
    ellipse.h = package.ellipse.h;

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
        pad.number = p.number;
        pad.width = p.width;
        pad.height = p.height;
        pad.radius = p.radius;
        pad.orientation = p.orientation;
        if (orientation == LEFT || orientation == RIGHT) {
            if (pad.orientation == UP)
                pad.orientation = RIGHT;
            else if (pad.orientation == RIGHT)
                pad.orientation = UP;
        }
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
