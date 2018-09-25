// circuitsymbol.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
#include "circuitsymbol.h"
#include "text.h"
#include <cstring>
#include <QJsonArray>

CircuitSymbol::CircuitSymbol(int type, int refX, int refY):
    type(type), refX(refX), refY(refY)
{
    init();
}

CircuitSymbol::CircuitSymbol(const QJsonObject &object)
{
    QString typeString(object["type"].toString());
    refX = object["refX"].toInt();
    refY = object["refY"].toInt();

    if (!findIndex(type, typeString,
                   circuitSymbolTypeString, circuitSymbolTypes))
        throw ExceptionData("Circuit symbol type error");

    init();
}

void CircuitSymbol::draw(QPainter &painter)
{
    for (int i = 0; i < linesNumber; i++)
        painter.drawLine(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
}

bool CircuitSymbol::exist(int x, int y)
{
    int dx = (circuitSymbolBorder[type][2] - circuitSymbolBorder[type][0]) / 2;
    int dy = (circuitSymbolBorder[type][3] - circuitSymbolBorder[type][1]) / 2;

    if (x >= (centerX - dx) && x <= (centerX + dx) &&
        y >= (centerY - dy) && y <= (centerY + dy))
        return true;

    return false;
}

void CircuitSymbol::init()
{
    centerX = refX + (circuitSymbolBorder[type][0] + circuitSymbolBorder[type][2]) / 2;
    centerY = refY + (circuitSymbolBorder[type][1] + circuitSymbolBorder[type][3]) / 2;
    center = (centerX << 16) + centerY;

    const int (*symbolImages[circuitSymbolTypes])[4] = {gnd};
    const int (*symbolImage)[4] = symbolImages[type];

    linesNumber = circuitSymbolLines[type];
    arcsNumber = 0;

    placeLines(symbolImage);
}

bool CircuitSymbol::inside(int leftX, int topY, int rightX, int bottomY)
{
    if (centerX >= leftX && centerX <= rightX &&
        centerY >= topY && centerY <= bottomY)
        return true;

    return false;
}

void CircuitSymbol::placeLines(const int (*image)[4])
{
    for (int i = 0; i < linesNumber; i++) {
        lines[i][0] = refX + (*((*image) + 4 * i));
        lines[i][1] = refY + (*((*image) + 4 * i + 1));
        lines[i][2] = refX + (*((*image) + 4 * i + 2));
        lines[i][3] = refY + (*((*image) + 4 * i + 3));
    }
}

QJsonObject CircuitSymbol::toJson()
{
    QString typeString(circuitSymbolTypeString[type]);

    QJsonObject object
    {
        {"type", typeString},
        {"refX", refX},
        {"refY", refY}
    };

    return object;
}
