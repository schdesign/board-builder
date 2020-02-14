// unit.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "text.h"
#include "unit.h"
#include <QJsonArray>

std::map<int, UnitSymbol> Unit::symbols;

QJsonObject UnitSymbol::toJson()
{
    QJsonArray unitEllipses;
    for (auto e : ellipses)
        unitEllipses.append(e.toJson());

    QJsonArray unitLines;
    for (auto l : lines)
        unitLines.append(l.toJson());

    QJsonObject object
    {
        {"border", border.toJson()},
        {"number", number + 1},
        {"referenceTextX", referenceTextX},
        {"referenceTextY", referenceTextY},
        {"refX", refX},
        {"refY", refY},
        {"ellipses", unitEllipses},
        {"lines", unitLines}
    };

    return object;
}

Unit::Unit(int deviceID, int number, int refX, int refY):
    deviceID(deviceID), number(number), refX(refX), refY(refY)
{
    init();
}

Unit::Unit(int deviceID, const QJsonObject &object):
    deviceID(deviceID)
{
    number = object["number"].toInt() - 1;
    QString referenceString = object["reference"].toString();
    refX = object["refX"].toInt();
    refY = object["refY"].toInt();

    init();
    reference = referenceString;
}

void Unit::addSymbol(const QJsonValue &value, int deviceID)
{
    UnitSymbol symbol;
    QJsonObject object = value.toObject();

    symbol.border.fromJson(object["border"]);
    symbol.number = object["number"].toInt() - 1;
    symbol.referenceTextX = object["referenceTextX"].toInt();
    symbol.referenceTextY = object["referenceTextY"].toInt();
    symbol.refX = object["refX"].toInt();
    symbol.refY = object["refY"].toInt();

    QJsonArray unitEllipses = object["ellipses"].toArray();
    QJsonArray unitLines = object["lines"].toArray();

    for (auto u : unitEllipses) {
        Ellipse ellipse(u);
        symbol.ellipses.push_back(ellipse);
    }

    for (auto u : unitLines) {
        Line line(u);
        symbol.lines.push_back(line);
    }

    symbol.nameID = (deviceID << 8) + symbol.number;
    Unit::symbols[symbol.nameID] = symbol;
}

void Unit::draw(QPainter &painter)
{
    for (auto l : lines)
        painter.drawLine(l.x1, l.y1, l.x2, l.y2);

    for (auto e : ellipses)
        painter.drawEllipse(e.x, e.y, e.w, e.h);

    painter.drawText(referenceTextX, referenceTextY, reference);
}

bool Unit::exist(int x, int y)
{
    int dx = (symbols[symbolNameID].border.rightX - symbols[symbolNameID].border.leftX) / 2;
    int dy = (symbols[symbolNameID].border.bottomY - symbols[symbolNameID].border.topY) / 2;

    if (x >= (centerX - dx) && x <= (centerX + dx) &&
        y >= (centerY - dy) && y <= (centerY + dy))
        return true;

    return false;
}

void Unit::init()
{
    symbolNameID = (deviceID << 8) + number;

    const UnitSymbol &symbol = symbols[symbolNameID];

    centerX = refX + (symbol.border.leftX + symbol.border.rightX) / 2;
    centerY = refY + (symbol.border.topY + symbol.border.bottomY) / 2;
    center = (centerX << 16) + centerY;

    referenceTextX = refX + symbol.referenceTextX;
    referenceTextY = refY + symbol.referenceTextY;

    Ellipse ellipse;
    for (auto se : symbol.ellipses) {
        ellipse.x = refX + se.x;
        ellipse.y = refY + se.y;
        ellipse.w = se.w;
        ellipse.h = se.h;
        ellipses.push_back(ellipse);
    }

    Line line;
    for (auto sl : symbol.lines) {
        line.x1 = refX + sl.x1;
        line.y1 = refY + sl.y1;
        line.x2 = refX + sl.x2;
        line.y2 = refY + sl.y2;
        lines.push_back(line);
    }
}

bool Unit::inside(int leftX, int topY, int rightX, int bottomY)
{
    if (centerX >= leftX && centerX <= rightX &&
        centerY >= topY && centerY <= bottomY)
        return true;

    return false;
}

QJsonObject Unit::toJson()
{
    QJsonObject object
    {
        {"number", number + 1},
        {"reference", reference},
        {"refX", refX},
        {"refY", refY}
    };

    return object;
}
