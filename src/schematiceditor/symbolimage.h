// symbolimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SYMBOL_IMAGE_H
#define SYMBOL_IMAGE_H

const int symbolTypes = 1;

enum SymbolType
{
    GROUND
};

const char symbolTypeString[symbolTypes][32] =
{
    "Ground"
};

// Border for symbol selection: left, top, right, bottom.
const int symbolBorder[symbolTypes][4] =
{
    {-8,2,8,12}     // gnd
};

const int symbolLines[symbolTypes] = {2};

const int gnd[2][4] = {{0,0,0,10}, {-6,10,6,10}};

#endif  // SYMBOL_IMAGE_H
