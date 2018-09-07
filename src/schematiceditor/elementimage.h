// elementimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ELEMENT_IMAGE_H
#define ELEMENT_IMAGE_H

const int elementTypes = 18;
const int elementReferences = 10;

enum ElementType
{
    BATTERY = 1, BUTTON, CAPACITOR, CELL, CORE_INDUCTOR,
    DIODE, INDUCTOR, LED, NPN_TRANSISTOR, PHOTODIODE,
    PNP_TRANSISTOR, POLAR_CAPACITOR, QUARTZ, RESISTOR_ANSI,
    RESISTOR_IEC, SHOTTKY, ZENER
};

const char elementTypeString[elementTypes][32] =
{
    "", "Battery", "Button", "Capacitor", "Cell", "Core Inductor",
    "Diode", "Inductor", "Led", "NPN Transistor", "Photodiode",
    "PNP Transistor", "Polar Capacitor", "Quarts", "Resistor ANSI",
    "Resistor IEC", "Shottky", "Zener"
};

const char elementReference[elementTypes][8] =
{
    "", "GB", "SB", "C", "G", "L", "VD", "L", "HL", "VT",
    "VD", "VT", "C", "ZQ", "R", "R", "VD", "VD"
};

enum ElementReferenceType
{
    C, G, GB, HL, L, R, SB, VD, VT, ZQ
};

const int elementReferenceTypes[elementTypes] =
{
    0, GB, SB, C, G, L, VD, L, HL, VT,
    VD, VT, C, ZQ, R, R, VD, VD
};

enum Orientation {UP, RIGHT, DOWN, LEFT};

const char elementOrientationString[4][32] =
{
    "Up", "Right", "Down", "Left"
};

#endif  // ELEMENT_IMAGE_H
