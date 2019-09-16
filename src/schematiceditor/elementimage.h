// elementimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ELEMENT_IMAGE_H
#define ELEMENT_IMAGE_H

const int elementTypes = 20;
const int elementReferences = 10;

enum ElementType
{
    BATTERY = 1, BUTTON, CAPACITOR, CELL, CORE_INDUCTOR,
    DIODE, INDUCTOR, LED, N_MOSFET, NPN_TRANSISTOR,
    PHOTODIODE, P_MOSFET, PNP_TRANSISTOR, POLAR_CAPACITOR,
    QUARTZ, RESISTOR_ANSI, RESISTOR_IEC, SCHOTTKY, ZENER
};

const char elementTypeString[elementTypes][32] =
{
    "", "Battery", "Button", "Capacitor", "Cell", "Core Inductor",
    "Diode", "Inductor", "Led", "N channel MOSFET", "NPN Transistor",
    "Photodiode", "P channel MOSFET", "PNP Transistor", "Polar Capacitor",
    "Quarts", "Resistor ANSI", "Resistor IEC", "Schottky", "Zener"
};

const char elementReference[elementTypes][8] =
{
    "", "GB", "SB", "C", "G", "L", "VD", "L", "HL", "VT",
    "VT", "VD", "VT", "VT", "C", "ZQ", "R", "R", "VD", "VD"
};

enum ElementReferenceType
{
    C, G, GB, HL, L, R, SB, VD, VT, ZQ
};

const int elementReferenceTypes[elementTypes] =
{
    0, GB, SB, C, G, L, VD, L, HL, VT,
    VT, VD, VT, VT, C, ZQ, R, R, VD, VD
};

enum Orientation {
    UP, RIGHT, DOWN, LEFT,
    UP_MIRROR, RIGHT_MIRROR, DOWN_MIRROR, LEFT_MIRROR
};

const char elementOrientationString[8][32] =
{
    "Up", "Right", "Down", "Left",
    "Up Mirror", "Right Mirror", "Down Mirror", "Left Mirror"
};

#endif  // ELEMENT_IMAGE_H
