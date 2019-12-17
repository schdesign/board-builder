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
        {"x", x},
        {"y", y}
    };

    return object;
}

void Package::clear()
{
    border.clear();
    ellipse.clear();
    nameTextHeight = 0;
    //nameTextAlignmentX = 0;
    //nameTextAlignmentY = 0;
    std::fill(nameTextX, nameTextX + 4, 0);
    std::fill(nameTextY, nameTextY + 4, 0);
    referenceTextHeight = 0;
    //referenceTextAlignmentX = 0;
    //referenceTextAlignmentY = 0;
    std::fill(referenceTextX, referenceTextX + 4, 0);
    std::fill(referenceTextY, referenceTextY + 4, 0);
    refX = 0;
    refY = 0;
    name.clear();
    type.clear();
    lines.clear();
    pads.clear();
}

QJsonObject Package::toJson()
{
    QJsonObject nameText
    {
        {"height", nameTextHeight},
        //{"alignmentX", nameTextAlignmentX},
        //{"alignmentY", nameTextAlignmentY},
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
        //{"alignmentX", referenceTextAlignmentX},
        //{"alignmentY", referenceTextAlignmentY},
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
