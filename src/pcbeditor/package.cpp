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

    nameTextHeight = nameTextObject["height"].toInt();
    nameTextX[0] = nameTextObject["upX"].toInt();
    nameTextY[0] = nameTextObject["upY"].toInt();
    nameTextX[1] = nameTextObject["rightX"].toInt();
    nameTextY[1] = nameTextObject["rightY"].toInt();
    nameTextX[2] = nameTextObject["downX"].toInt();
    nameTextY[2] = nameTextObject["downY"].toInt();
    nameTextX[3] = nameTextObject["leftX"].toInt();
    nameTextY[3] = nameTextObject["leftY"].toInt();

    referenceTextHeight = referenceTextObject["height"].toInt();
    referenceTextX[0] = referenceTextObject["upX"].toInt();
    referenceTextY[0] = referenceTextObject["upY"].toInt();
    referenceTextX[1] = referenceTextObject["rightX"].toInt();
    referenceTextY[1] = referenceTextObject["rightY"].toInt();
    referenceTextX[2] = referenceTextObject["downX"].toInt();
    referenceTextY[2] = referenceTextObject["downY"].toInt();
    referenceTextX[3] = referenceTextObject["leftX"].toInt();
    referenceTextY[3] = referenceTextObject["leftY"].toInt();

    refX = 0;
    refY = 0;

    name = object["name"].toString();
    nameTextAlignH = object["nameTextAlignH"].toString();
    nameTextAlignV = object["nameTextAlignV"].toString();
    referenceTextAlignH = object["referenceTextAlignH"].toString();
    referenceTextAlignV = object["referenceTextAlignV"].toString();
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
    nameTextHeight = 0;
    std::fill(nameTextX, nameTextX + 4, 0);
    std::fill(nameTextY, nameTextY + 4, 0);
    referenceTextHeight = 0;
    std::fill(referenceTextX, referenceTextX + 4, 0);
    std::fill(referenceTextY, referenceTextY + 4, 0);
    refX = 0;
    refY = 0;
    name.clear();
    nameTextAlignH.clear();
    nameTextAlignV.clear();
    referenceTextAlignH.clear();
    referenceTextAlignV.clear();
    type.clear();
    ellipses.clear();
    lines.clear();
    pads.clear();
    padTypesParams.clear();
}

QJsonObject Package::toJson()
{
    QJsonObject nameText
    {
        {"height", nameTextHeight},
        {"upX", nameTextX[0]},
        {"upY", nameTextY[0]},
        {"rightX", nameTextX[1]},
        {"rightY", nameTextY[1]},
        {"downX", nameTextX[2]},
        {"downY", nameTextY[2]},
        {"leftX", nameTextX[3]},
        {"leftY", nameTextY[3]},
        {"nameTextAlignH", nameTextAlignH},
        {"nameTextAlignV", nameTextAlignV}
    };

    QJsonObject referenceText
    {
        {"height", referenceTextHeight},
        {"upX", referenceTextX[0]},
        {"upY", referenceTextY[0]},
        {"rightX", referenceTextX[1]},
        {"rightY", referenceTextY[1]},
        {"downX", referenceTextX[2]},
        {"downY", referenceTextY[2]},
        {"leftX", referenceTextX[3]},
        {"leftY", referenceTextY[3]},
        {"referenceTextAlignH", referenceTextAlignH},
        {"referenceTextAlignV", referenceTextAlignV}
    };

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
        {"nameText", nameText},
        {"referenceText", referenceText},
        {"type", type},
        {"ellipses", ellipsesArray},
        {"lines", linesArray},
        {"pads", padsArray},
        {"padTypesParams", padTypesParamsArray}
    };

    return object;
}
