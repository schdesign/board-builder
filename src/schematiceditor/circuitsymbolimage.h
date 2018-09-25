// circuitsymbolimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef CIRCUITSYMBOLIMAGE_H
#define CIRCUITSYMBOLIMAGE_H

const int circuitSymbolTypes = 1;

enum CircuitSymbolType
{
    GROUND
};

const char circuitSymbolTypeString[circuitSymbolTypes][32] =
{
    "Ground"
};

// Border for symbol selection: left, top, right, bottom.
const int circuitSymbolBorder[circuitSymbolTypes][4] =
{
    {-8,2,8,12}     // gnd
};

const int circuitSymbolLines[circuitSymbolTypes] = {2};

const int gnd[2][4] = {{0,0,0,10}, {-6,10,6,10}};

#endif  // CIRCUITSYMBOLIMAGE_H
