// circuitsymbolimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef CIRCUITSYMBOLIMAGE_H
#define CIRCUITSYMBOLIMAGE_H

constexpr int circuitSymbolTypes = 2;

enum CircuitSymbolType
{
    GROUND, GROUND_IEC
};

const char circuitSymbolTypeString[circuitSymbolTypes][32] =
{
    "Ground", "Ground IEC"
};

// Border for symbol selection: left, top, right, bottom.
constexpr int circuitSymbolBorder[circuitSymbolTypes][4] =
{
    {-8,2,8,12}, {-12,2,12,16}
};

constexpr int circuitSymbolLinesNumber[circuitSymbolTypes] = {2, 4};

constexpr int circuitSymbolLines[circuitSymbolTypes][16][4] =
{
    {{0,0,0,10}, {-6,10,6,10}},                               // Ground
    {{0,0,0,10}, {-10,10,10,10}, {-6,14,6,14}, {-3,18,3,18}}  // Ground IEC
};

#endif  // CIRCUITSYMBOLIMAGE_H
