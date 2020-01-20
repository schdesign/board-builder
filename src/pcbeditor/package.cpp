// package.cpp
// Copyright (C) 2019 Alexander Karpeko

#include "package.h"
#include <algorithm>
#include <QJsonArray>

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
}

void Package::clear()
{
    border.clear();
    name.clear();
    type.clear();
    ellipses.clear();
    lines.clear();
    pads.clear();
    padTypesParams.clear();
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
