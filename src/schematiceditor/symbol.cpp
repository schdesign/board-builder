// symbol.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
#include "symbol.h"
#include "text.h"
#include <cstring>
#include <QJsonArray>

Symbol::Symbol(int type, int refX, int refY):
    type(type), refX(refX), refY(refY)
{
    init();
}

Symbol::Symbol(const QJsonObject &object)
{
    QString typeString(object["type"].toString());
    refX = object["refX"].toInt();
    refY = object["refY"].toInt();

    if (!findIndex(type, typeString, symbolTypeString, symbolTypes))
        throw ExceptionData("Symbol type error");

    init();
}

void Symbol::draw(QPainter &painter)
{
    for (int i = 0; i < linesNumber; i++)
        painter.drawLine(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
}

bool Symbol::exist(int x, int y)
{
    int dx = (symbolBorder[type][2] - symbolBorder[type][0]) / 2;
    int dy = (symbolBorder[type][3] - symbolBorder[type][1]) / 2;

    if (x >= (centerX - dx) && x <= (centerX + dx) &&
        y >= (centerY - dy) && y <= (centerY + dy))
        return true;

    return false;
}

void Symbol::init()
{
    centerX = refX + (symbolBorder[type][0] + symbolBorder[type][2]) / 2;
    centerY = refY + (symbolBorder[type][1] + symbolBorder[type][3]) / 2;
    center = (centerX << 16) + centerY;

    const int (*symbolImages[symbolTypes])[4] = {gnd};
    const int (*symbolImage)[4] = symbolImages[type];

    linesNumber = symbolLines[type];
    arcsNumber = 0;

    placeLines(symbolImage);
}

bool Symbol::inside(int leftX, int topY, int rightX, int bottomY)
{
    if (centerX >= leftX && centerX <= rightX &&
        centerY >= topY && centerY <= bottomY)
        return true;

    return false;
}

void Symbol::placeLines(const int (*image)[4])
{
    for (int i = 0; i < linesNumber; i++) {
        lines[i][0] = refX + (*((*image) + 4 * i));
        lines[i][1] = refY + (*((*image) + 4 * i + 1));
        lines[i][2] = refX + (*((*image) + 4 * i + 2));
        lines[i][3] = refY + (*((*image) + 4 * i + 3));
    }
}

QJsonObject Symbol::toJson()
{
    QString typeString(symbolTypeString[type]);

    QJsonObject object
    {
        {"type", typeString},
        {"refX", refX},
        {"refY", refY}
    };

    return object;
}
