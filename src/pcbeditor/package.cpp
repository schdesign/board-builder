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
