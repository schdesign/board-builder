// package.cpp
// Copyright (C) 2019 Alexander Karpeko

#include "package.h"
#include <algorithm>
#include <cmath>
#include <QJsonArray>

bool Pad::exist(int x_, int y_)
{
    int dx = width / 2;
    int dy = height / 2;

    if (dx > 0 && dy > 0)
        if (x_ >= (x - dx) && x_ <= (x + dx) &&
            y_ >= (y - dy) && y_ <= (y + dy))
            return true;

    int r = diameter / 2;

    if (r > 0)
        if (lround(hypot((x_ - x), (y_ - y))) < r)
            return true;

    return false;
}

QJsonObject Pad::toJson()
{
    QJsonObject object
    {
        {"orientation", orientation},
        {"typeNumber", typeNumber},
        {"x", x},
        {"y", y}
    };

    return object;
}

QJsonObject PadTypeParams::toJson()
{
    QJsonObject object
    {
        {"diameter", diameter},
        {"height", height},
        {"innerDiameter", innerDiameter},
        {"width", width}
    };

    return object;
}

Package::Package(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    QJsonArray ellipsesArray = object["ellipses"].toArray();
    QJsonArray linesArray = object["lines"].toArray();
    QJsonArray padsArray = object["pads"].toArray();
    QJsonArray padTypesParamsArray = object["padTypesParams"].toArray();
    QJsonObject nameTextObject = object["nameText"].toObject();
    QJsonObject referenceTextObject = object["referenceText"].toObject();

    border.fromJson(object["border"]);

    centerX = (border.leftX + border.rightX) / 2;
    centerY = (border.topY + border.bottomY) / 2;

    name = object["name"].toString();
    type = object["type"].toString();

    for (auto e : ellipsesArray) {
        Ellipse ellipse(e);
        ellipses.push_back(ellipse);
    }

    for (auto l : linesArray) {
        Line line(l);
        lines.push_back(line);
    }

    for (auto p : padTypesParamsArray) {
        QJsonObject paramsObject = p.toObject();
        PadTypeParams params;
        params.diameter = paramsObject["diameter"].toInt();
        params.height = paramsObject["height"].toInt();
        params.innerDiameter = paramsObject["innerDiameter"].toInt();
        params.width = paramsObject["width"].toInt();
        padTypesParams.push_back(params);
    }

    for (int i = 0; i < padsArray.size(); i++) {
        QJsonObject packagePad = padsArray[i].toObject();
        Pad pad;
        pad.net = 0;
        pad.number = i + 1;
        pad.orientation = packagePad["orientation"].toInt();
        pad.typeNumber = packagePad["typeNumber"].toInt();
        pad.diameter = padTypesParams[pad.typeNumber].diameter;
        pad.height = padTypesParams[pad.typeNumber].height;
        pad.innerDiameter = padTypesParams[pad.typeNumber].innerDiameter;
        pad.width = padTypesParams[pad.typeNumber].width;
        pad.x = packagePad["x"].toInt();
        pad.y = packagePad["y"].toInt();
        pads.push_back(pad);
    }

    findOuterBorder();
}

void Package::clear()
{
    centerX = 0;
    centerY = 0;
    outerBorderCenterX = 0;
    outerBorderCenterY = 0;
    border.clear();
    outerBorder.clear();
    name.clear();
    type.clear();
    ellipses.clear();
    lines.clear();
    pads.clear();
    padTypesParams.clear();
}

void Package::findOuterBorder()
{
    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;

    for (auto e : ellipses) {
        if (minX > e.x - e.w / 2) minX = e.x - e.w / 2;
        if (maxX < e.x + e.w / 2) maxX = e.x + e.w / 2;
        if (minY > e.y - e.h / 2) minY = e.y - e.h / 2;
        if (maxY < e.y + e.h / 2) maxY = e.y + e.h / 2;
    }

    for (auto l : lines) {
        if (minX > l.x1) minX = l.x1;
        if (minX > l.x2) minX = l.x2;
        if (maxX < l.x1) maxX = l.x1;
        if (maxX < l.x2) maxX = l.x2;
        if (minY > l.y1) minY = l.y1;
        if (minY > l.y2) minY = l.y2;
        if (maxY < l.y1) maxY = l.y1;
        if (maxY < l.y2) maxY = l.y2;
    }

    for (auto p : pads) {
        int w = p.width;
        int h = p.height;
        if (w == 0 || h == 0) {
            w = p.diameter;
            h = p.diameter;
        }
        if (minX > p.x - w / 2) minX = p.x - w / 2;
        if (maxX < p.x + w / 2) maxX = p.x + w / 2;
        if (minY > p.y - h / 2) minY = p.y - h / 2;
        if (maxY < p.y + h / 2) maxY = p.y + h / 2;
    }

    outerBorder.leftX = minX;
    outerBorder.rightX = maxX;
    outerBorder.topY = minY;
    outerBorder.bottomY = maxY;

    outerBorderCenterX = (minX + maxX) / 2;
    outerBorderCenterY = (minY + maxY) / 2;
}

QJsonObject Package::toJson()
{
    QJsonArray ellipsesArray;
    for (auto e : ellipses)
        ellipsesArray.append(e.toJson());

    QJsonArray linesArray;
    for (auto l : lines)
        linesArray.append(l.toJson());

    QJsonArray padsArray;
    for (auto p : pads)
        padsArray.append(p.toJson());

    QJsonArray padTypesParamsArray;
    for (auto p : padTypesParams)
        padTypesParamsArray.append(p.toJson());

    QJsonObject object
    {
        {"border", border.toJson()},
        {"name", name},
        {"type", type},
        {"ellipses", ellipsesArray},
        {"lines", linesArray},
        {"pads", padsArray},
        {"padTypesParams", padTypesParamsArray}
    };

    return object;
}
