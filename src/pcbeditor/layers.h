// layers.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef LAYERS_H
#define LAYERS_H

#include <QColor>
#include <QString>

const int layersNumber = 11;

enum LayerNames
{
    FRONT, BACK, BORDER, PAD,
    FRONT_POLYGON, BACK_POLYGON, FRONT_VIA, BACK_VIA,
    PACKAGE, REFERENCE, NAME
};

const QString layerNameString[layersNumber] =
{
    "Front", "Back", "Border", "Pad",
    "Front polygon", "Back polygon", "Front via", "Back via",
    "Package", "Reference", "Name"
};

const int layerColors[layersNumber][3] =
{
    {255, 0, 0}, {0, 255, 0}, {200, 200, 0}, {255, 0, 0},
    {255, 0, 0}, {0, 255, 0}, {255, 0, 0}, {0, 255, 0},
    {0, 150, 0}, {0, 150, 0}, {0, 150, 0}
};

class Layers
{
public:
    Layers();

    int draw;   // bit enabled: draw layer
    int edit;   // number of editing layer
    QColor color[layersNumber];
};

#endif  // LAYERS_H
