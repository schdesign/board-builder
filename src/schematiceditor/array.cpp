// array.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "array.h"
#include "exceptiondata.h"
#include "function.h"
#include "text.h"
#include <cmath>
#include <QJsonArray>

std::map<int, ArraySymbol> Array::symbols;

QJsonObject ArraySymbol::toJson()
{
    refX = 0;
    refY = 0;

    QString typeString(arrayTypeString[type]);

    QJsonArray arrayLines;
    for (auto l : lines)
        arrayLines.append(l.toJson());

    QJsonArray arrayPins;
    for (auto p : pins)
        arrayPins.append(p.toJson());

    QJsonObject object
    {
        {"showPinName", showPinName},
        {"showPinNumber", showPinNumber},
        {"border", border.toJson()},
        {"deltaY", deltaY},
        {"referenceTextX", intArrayToJson(referenceTextX, 2)},
        {"referenceTextY", intArrayToJson(referenceTextY, 2)},
        {"refX", refX},
        {"refY", refY},
        {"type", typeString},
        {"nameTextX", intArrayToJson(nameTextX, 2)},
        {"nameTextY", intArrayToJson(nameTextY, 2)},
        {"pinNameTextX", intArrayToJson(pinNameTextX, 2)},
        {"pinNameTextY", intArrayToJson(pinNameTextY, 2)},
        {"pinNumberTextX", intArrayToJson(pinNumberTextX, 2)},
        {"pinNumberTextY", intArrayToJson(pinNumberTextY, 2)},
        {"name", name},
        {"lines", arrayLines},
        {"pins", arrayPins}
   };

    return object;
}

Array::Array(int type, int number, int refX, int refY, int orientation):
    number(number), orientation(orientation), refX(refX), refY(refY), type(type)
{
    init();
}

Array::Array(const QJsonObject &object)
{
    QString typeString(object["type"].toString());
    QString orientationString(object["orientation"].toString());
    refX = object["refX"].toInt();
    refY = object["refY"].toInt();
    reference = object["reference"].toString();
    name = object["name"].toString();
    package = object["package"].toString();
    QJsonArray arrayPins = object["pins"].toArray();
    number = arrayPins.size();

    for (auto a : arrayPins)
        pinName.push_back(a.toString());

    if (!findIndex(type, typeString, arrayTypeString, arrayTypes))
        throw ExceptionData("Array type error");

    if (!findIndex(orientation, orientationString, arrayOrientationString, 2))
        throw ExceptionData("Array orientation error");

    init();
}

void Array::addSymbol(const QJsonValue &value)
{
    ArraySymbol symbol;
    QJsonObject object = value.toObject();

    symbol.showPinName = object["showPinName"].toBool();
    symbol.showPinNumber = object["showPinNumber"].toBool();
    symbol.border.fromJson(object["border"]);
    symbol.deltaY = object["deltaY"].toInt();
    jsonToIntArray(object["referenceTextX"], symbol.referenceTextX, 2);
    jsonToIntArray(object["referenceTextY"], symbol.referenceTextY, 2);
    symbol.refX = object["refX"].toInt();
    symbol.refY = object["refY"].toInt();

    QString typeString(object["type"].toString());
    if (!findIndex(symbol.type, typeString, arrayTypeString, arrayTypes))
        throw ExceptionData("Array type error");
    symbol.reference = arrayReference[symbol.type];

    jsonToIntArray(object["nameTextX"], symbol.nameTextX, 2);
    jsonToIntArray(object["nameTextY"], symbol.nameTextY, 2);
    jsonToIntArray(object["pinNameTextX"], symbol.pinNameTextX, 2);
    jsonToIntArray(object["pinNameTextY"], symbol.pinNameTextY, 2);
    jsonToIntArray(object["pinNumberTextX"], symbol.pinNumberTextX, 2);
    jsonToIntArray(object["pinNumberTextY"], symbol.pinNumberTextY, 2);

    symbol.name = object["name"].toString();
    QJsonArray arrayLines = object["lines"].toArray();
    QJsonArray arrayPins = object["pins"].toArray();

    for (auto a : arrayLines) {
        Line line(a);
        symbol.lines.push_back(line);
    }

    for (auto a : arrayPins) {
        Point pin(a);
        symbol.pins.push_back(pin);
    }

    Array::symbols[symbol.type] = symbol;
}

void Array::draw(QPainter &painter)
{
    int dx;
    QString str;

    for (auto l : lines)
        painter.drawLine(l.x1, l.y1, l.x2, l.y2);

    if (name.size())
        painter.drawText(nameTextX, nameTextY + deltaY * number, name);

    if (showPinName) {
        for (uint i = 0; i < pinName.size(); i++)
            painter.drawText(pinNameTextX, pinNameTextY + deltaY * i, pinName[i]);
    }

    if (showPinNumber) {
        dx = 0;
        for (int i = 0; i < number; i++) {
            if (i == 9)
                dx = -6;
            painter.drawText(pinNumberTextX + dx, pinNumberTextY + deltaY * i, str.setNum(i + 1));
        }
    }

    painter.drawText(referenceTextX, referenceTextY, reference);
}

bool Array::exist(int x, int y)
{
    int dx = abs(border.rightX - border.leftX) / 2;
    int dy = abs(border.bottomY - border.topY) / 2;

    if (x >= (centerX - dx) && x <= (centerX + dx) &&
        y >= (centerY - dy) && y <= (centerY + dy))
        return true;

    return false;
}

void Array::init()
{
    const ArraySymbol &symbol = symbols[type];
    const int c[2] = {1, -1};

    showPinName = symbol.showPinName;
    showPinNumber = symbol.showPinNumber;

    deltaY = symbol.deltaY;

    int t = orientation;
    limit(t, 0, 1);

    border.leftX = refX + c[t] * symbol.border.leftX;
    border.topY = refY + symbol.border.topY;
    border.rightX = refX + c[t] * symbol.border.rightX;
    border.bottomY = refY + symbol.border.bottomY + number * deltaY;

    centerX = (border.leftX + border.rightX) / 2;
    centerY = (border.topY + border.bottomY) / 2;
    center = (centerX << 16) + centerY;

    referenceTextX = refX + symbol.referenceTextX[t];
    referenceTextY = refY + symbol.referenceTextY[t];
    referenceType = arrayReferenceTypes[symbol.type];

    nameTextX = refX + symbol.nameTextX[t];
    nameTextY = refY + symbol.nameTextY[t];

    pinNameTextX = refX + symbol.pinNameTextX[t];
    pinNameTextY = refY + symbol.pinNameTextY[t];

    pinNumberTextX = refX + symbol.pinNumberTextX[t];
    pinNumberTextY = refY + symbol.pinNumberTextY[t];

    if (reference.isEmpty())
        reference = symbol.reference;

    if (showPinName)
        pinName.resize(number);

    Line line;
    for (int i = 0; i < number; i++)
        for (auto sl : symbol.lines) {
            line.x1 = refX + c[t] * sl.x1;
            line.y1 = refY + sl.y1 + deltaY * i;
            line.x2 = refX + c[t] * sl.x2;
            line.y2 = refY + sl.y2 + deltaY * i;
            lines.push_back(line);
        }

    Point pin;
    for (int i = 0; i < number; i++)
        for (auto sp : symbol.pins) {
            pin.x = refX + c[t] * sp.x;
            pin.y = refY + sp.y + symbol.deltaY * i;
            pins.push_back(pin);
        }
}

bool Array::inside(int leftX, int topY, int rightX, int bottomY)
{
    if (centerX >= leftX && centerX <= rightX &&
        centerY >= topY && centerY <= bottomY)
        return true;

    return false;
}

QJsonObject Array::toJson()
{
    QString typeString(arrayTypeString[type]);
    QString orientationString(arrayOrientationString[orientation]);

    QJsonArray arrayPins;
    for (auto p : pinName)
        arrayPins.append(p);

    QJsonObject object
    {
        {"type", typeString},
        {"orientation", orientationString},
        {"refX", refX},
        {"refY", refY},
        {"reference", reference},
        {"name", name},
        {"package", package},
        {"pins", arrayPins}
    };

    return object;
}

QJsonObject Array::writeSymbols()
{
    QJsonArray arraySymbols;
    for (auto a : Array::symbols)
        arraySymbols.append(a.second.toJson());

    QJsonObject object
    {
        {"arrays", arraySymbols},
        {"object", "symbols"}
    };

    return object;
}
