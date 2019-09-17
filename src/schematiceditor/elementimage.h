// elementimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ELEMENT_IMAGE_H
#define ELEMENT_IMAGE_H

constexpr int elementTypes = 28;
constexpr int elementReferences = 10;

enum ElementType
{
    BATTERY = 1, BUTTON, CAPACITOR, CELL, CORE_INDUCTOR,
    DIODE, COMMON_ANODE_DIODES, COMMON_ANODE_SCHOTTKY,
    COMMON_CATHODE_DIODES, COMMON_CATHODE_SCHOTTKY, INDUCTOR,
    LED, N_MOSFET, NPN_TRANSISTOR, PHOTODIODE, P_MOSFET,
    PNP_TRANSISTOR, POLAR_CAPACITOR, QUARTZ, RESISTOR_ANSI,
    RESISTOR_IEC, SCHOTTKY, SERIES_DIODES, SERIES_SCHOTTKY,
    SINGLE_DIODE, SINGLE_SCHOTTKY, ZENER
};

const char elementTypeString[elementTypes][32] =
{
    "", "Battery", "Button", "Capacitor", "Cell", "Core Inductor",
    "Diode", "Common Anode Diodes", "Common Anode Schottky",
    "Common Cathode Diodes", "Common Cathode Schottky", "Inductor",
    "Led", "N channel MOSFET", "NPN Transistor", "Photodiode",
    "P channel MOSFET", "PNP Transistor", "Polar Capacitor", "Quarts",
    "Resistor ANSI", "Resistor IEC", "Schottky", "Series Diodes",
    "Series Schottky", "Single Diodes", "Single Schottky", "Zener"
};

const char elementReference[elementTypes][8] =
{
    "", "GB", "SB", "C", "G", "L", "VD", "VD", "VD", "VD",
    "VD", "L", "HL", "VT", "VT", "VD", "VT", "VT", "C", "ZQ",
    "R", "R", "VD", "VD", "VD", "VD", "VD", "VD"
};

enum ElementReferenceType
{
    C, G, GB, HL, L, R, SB, VD, VT, ZQ
};

constexpr int elementReferenceTypes[elementTypes] =
{
    0, GB, SB, C, G, L, VD, VD, VD, VD,
    VD, L, HL, VT, VT, VD, VT, VT, C, ZQ,
    R, R, VD, VD, VD, VD, VD, VD
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
