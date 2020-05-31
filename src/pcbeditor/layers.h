// layers.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef LAYERS_H
#define LAYERS_H

#include <QColor>
#include <QString>

constexpr int layersNumber = 21;

enum LayerNames
{
    BORDER_LAYER, BOTTOM_LAYER, BOTTOM_MASK_LAYER, BOTTOM_NAME_LAYER,
    BOTTOM_PACKAGE_LAYER, BOTTOM_PAD_LAYER, BOTTOM_PASTE_LAYER, BOTTOM_POLYGON_LAYER,
    BOTTOM_REFERENCE_LAYER, BOTTOM_SILK_LAYER, BOTTOM_VIA_LAYER, TOP_LAYER,
    TOP_MASK_LAYER, TOP_NAME_LAYER, TOP_PACKAGE_LAYER, TOP_PAD_LAYER, TOP_PASTE_LAYER,
    TOP_POLYGON_LAYER, TOP_REFERENCE_LAYER, TOP_SILK_LAYER, TOP_VIA_LAYER
};

const QString layerNameString[layersNumber] =
{
    "Border", "Bottom", "Bottom mask", "Bottom name", "Bottom package", "Bottom pad",
    "Bottom paste", "Bottom polygon", "Bottom reference", "Bottom silk", "Bottom via",
    "Top", "Top mask", "Top name", "Top package", "Top pad", "Top paste", "Top polygon",
    "Top reference", "Top silk", "Top via"
};

const int layerColors[layersNumber][3] =
{
    {200, 200, 0}, {0, 255, 0}, {200, 255, 100}, {0, 150, 0}, {0, 150, 0}, {0, 255, 0},
    {150, 150, 150}, {0, 255, 0}, {0, 150, 0}, {0, 255, 255}, {0, 255, 0}, {255, 0, 0},
    {255, 255, 0}, {0, 150, 0}, {0, 150, 0}, {255, 0, 0}, {150, 150, 150}, {255, 0, 0},
    {0, 150, 0}, {0, 255, 255}, {255, 0, 0}
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
