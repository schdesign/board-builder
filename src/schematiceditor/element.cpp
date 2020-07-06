// element.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "element.h"
#include "exceptiondata.h"
#include "function.h"
#include "text.h"
#include <cmath>
#include <QJsonArray>

std::map<int, ElementSymbol> Element::symbols;

QJsonObject ElementSymbol::toJson()
{
    refX = 0;
    refY = 0;

    QString typeString(elementTypeString[type]);

    QJsonArray elementArcs;
    for (auto a : arcs)
        elementArcs.append(a.toJson());

    QJsonArray elementLines;
    for (auto l : lines)
        elementLines.append(l.toJson());

    QJsonArray elementPins;
    for (auto p : pins)
        elementPins.append(p.toJson());

    int size = orientations;

    if (!mirror)
        size /= 2;

    QJsonObject object
    {
        {"border", border.toJson()},
        {"mirror", mirror},
        {"referenceTextX", intArrayToJson(referenceTextX, size)},
        {"referenceTextY", intArrayToJson(referenceTextY, size)},
        {"refX", refX},
        {"refY", refY},
        {"type", typeString},
        {"valueTextX", intArrayToJson(valueTextX, size)},
        {"valueTextY", intArrayToJson(valueTextY, size)},
        {"name", name},
        {"arcs", elementArcs},
        {"lines", elementLines},
        {"pins", elementPins}
   };

    return object;
}

Element::Element(int type, int refX, int refY, int orientation, QString value):
    orientation(orientation), refX(refX), refY(refY), type(type), value(value)
{
    init();
}

Element::Element(const QJsonObject &object)
{
    mirror = object["mirror"].toBool();
    QString orientationString(object["orientation"].toString());
    packageName = object["package"].toString();
    padsMap = object["padsMap"].toInt();
    refX = object["refX"].toInt();
    refY = object["refY"].toInt();
    QString referenceString(object["reference"].toString());
    QString typeString(object["type"].toString());
    value = object["value"].toString();

    if (!findIndex(type, typeString, elementTypeString, elementTypes))
        throw ExceptionData("Element type error");

    if (!findIndex(orientation, orientationString, elementOrientationString, orientations))
        throw ExceptionData("Element orientation error");

    init();
    reference = referenceString;
}

void Element::addSymbol(const QJsonValue &value)
{
    ElementSymbol symbol;
    QJsonObject object = value.toObject();

    int size = orientations;

    symbol.mirror = object["mirror"].toBool();

    if (!symbol.mirror)
        size /= 2;

    symbol.border.fromJson(object["border"]);
    jsonToIntArray(object["referenceTextX"], symbol.referenceTextX, size);
    jsonToIntArray(object["referenceTextY"], symbol.referenceTextY, size);
    symbol.refX = object["refX"].toInt();
    symbol.refY = object["refY"].toInt();

    QString typeString(object["type"].toString());
    if (!findIndex(symbol.type, typeString, elementTypeString, elementTypes))
        throw ExceptionData("Element type error");
    symbol.reference = elementReference[symbol.type];

    jsonToIntArray(object["valueTextX"], symbol.valueTextX, size);
    jsonToIntArray(object["valueTextY"], symbol.valueTextY, size);

    symbol.name = object["name"].toString();
    QJsonArray arrayArcs = object["arcs"].toArray();
    QJsonArray arrayLines = object["lines"].toArray();
    QJsonArray arrayPins = object["pins"].toArray();

    for (auto a : arrayArcs) {
        Arc arc(a);
        symbol.arcs.push_back(arc);
    }

    for (auto a : arrayLines) {
        Line line(a);
        symbol.lines.push_back(line);
    }

    for (auto a : arrayPins) {
        Point pin(a);
        symbol.pins.push_back(pin);
    }

    Element::symbols[symbol.type] = symbol;
}

void Element::defaultPadsMap()
{
    padsMap = 0;

    for (auto e : equalPinsTypes)
        if (type == e) {
            if (pins.size() == 2)
                padsMap = 12;
            break;
        }
}

void Element::draw(QPainter &painter, bool showText, bool showPinNumbers)
{
    for (auto a : arcs)  // angle unit: 1/16th of degree
        painter.drawArc(a.x, a.y, a.w, a.h, a.startAngle << 4, a.spanAngle << 4);

    for (auto l : lines)
        painter.drawLine(l.x1, l.y1, l.x2, l.y2);

    if (showText) {
        painter.drawText(referenceTextX, referenceTextY, reference);
        painter.drawText(valueTextX, valueTextY, value);
    }

    if (showPinNumbers)
        for (int i = 0; i < pins.size(); i++) {
            int dx = pins[i].x - centerX;
            int dy = pins[i].y - centerY;
            int x = pins[i].x;
            int y = pins[i].y;
            if (abs(dx) > abs(dy)) {
                if (dx > 0) x += 10;
                else x -= 15;
                y += 3 * (dy / abs(dy));
            }
            else {
                if (dy > 0) y += 20;
                else y -= 10;
                x -= 3 * (dx / abs(dx));
            }
            painter.drawText(x, y, QString::number(i+1));
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

void Element::init()
{
    const ElementSymbol &symbol = symbols[type];

    constexpr int a[orientations][2] =  // arc
    {
        {1,0}, {0,1}, {1,0}, {0,1},
        {1,0}, {0,1}, {1,0}, {0,1}
    };

    constexpr int l[orientations][4] =  // line
    {
        {1,0,0,1}, {0,-1,1,0}, {-1,0,0,-1}, {0,1,-1,0},
        {-1,0,0,1}, {0,1,1,0}, {1,0,0,-1}, {0,-1,-1,0}
    };

    mirror = symbol.mirror;

    int t = orientation;

    if (t < 0 || (t > 3 && !mirror) || t > 7)
        t = 0;

    border.leftX = refX + l[t][0] * symbol.border.leftX + l[t][1] * symbol.border.topY;
    border.topY = refY + l[t][2] * symbol.border.leftX + l[t][3] * symbol.border.topY;
    border.rightX = refX + l[t][0] * symbol.border.rightX + l[t][1] * symbol.border.bottomY;
    border.bottomY = refY + l[t][2] * symbol.border.rightX + l[t][3] * symbol.border.bottomY;

    centerX = (border.leftX + border.rightX) / 2;
    centerY = (border.topY + border.bottomY) / 2;
    center = (centerX << 16) + centerY;

    referenceTextX = refX + symbol.referenceTextX[t];
    referenceTextY = refY + symbol.referenceTextY[t];

    referenceType = elementReferenceTypes[type];

    valueTextX = refX + symbol.valueTextX[t];
    valueTextY = refY + symbol.valueTextY[t];

    reference = symbol.reference;

    Arc arc;
    for (auto sa : symbol.arcs) {
        int arcCenterX = sa.x + sa.w / 2;
        int arcCenterY = sa.y + sa.h / 2;
        int arcCenterX2 = refX + l[t][0] * arcCenterX + l[t][1] * arcCenterY;
        int arcCenterY2 = refY + l[t][2] * arcCenterX + l[t][3] * arcCenterY;
        arc.x = arcCenterX2 - a[t][0] * sa.w / 2 - a[t][1] * sa.h / 2;
        arc.y = arcCenterY2 - a[t][1] * sa.w / 2 - a[t][0] * sa.h / 2;
        arc.w = a[t][0] * sa.w + a[t][1] * sa.h;
        arc.h = a[t][1] * sa.w + a[t][0] * sa.h;
        arc.startAngle = sa.startAngle - 90 * t;
        arc.spanAngle = sa.spanAngle;
        arcs.push_back(arc);
    }

    Line line;
    for (auto sl : symbol.lines) {
        line.x1 = refX + l[t][0] * sl.x1 + l[t][1] * sl.y1;
        line.y1 = refY + l[t][2] * sl.x1 + l[t][3] * sl.y1;
        line.x2 = refX + l[t][0] * sl.x2 + l[t][1] * sl.y2;
        line.y2 = refY + l[t][2] * sl.x2 + l[t][3] * sl.y2;
        lines.push_back(line);
    }

    Point pin;
    for (auto sp : symbol.pins) {
        pin.x = refX + l[t][0] * sp.x + l[t][1] * sp.y;
        pin.y = refY + l[t][2] * sp.x + l[t][3] * sp.y;
        pins.push_back(pin);
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
    QString typeString(elementTypeString[type]);
    QString orientationString(elementOrientationString[orientation]);

    QJsonObject object
    {
        {"mirror", mirror},
        {"orientation", orientationString},
        {"package", packageName},
        {"padsMap", padsMap},
        {"refX", refX},
        {"refY", refY},
        {"reference", reference},
        {"type", typeString},
        {"value", value}
    };

    return object;
}

QJsonObject Element::writeSymbols()
{
    QJsonArray elementSymbols;
    for (auto e : Element::symbols)
        elementSymbols.append(e.second.toJson());

    QJsonObject object
    {
        {"elements", elementSymbols},
        {"object", "symbols"}
    };

    return object;
}
