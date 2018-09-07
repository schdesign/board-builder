// layers.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "layers.h"

Layers::Layers()
{
    for (int i = 0; i < layersNumber; i++)
        color[i].setRgb(layerColors[i][0], layerColors[i][1], layerColors[i][2]);

    draw = 0x7ff;   // show all layers
    edit = -1;      // read only mode
}
