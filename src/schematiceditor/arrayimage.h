// arrayimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ARRAY_IMAGE_H
#define ARRAY_IMAGE_H

const int arrayTypes = 3;
const int arrayReferences = 2;

enum ArrayType {ARRAY_FCON, ARRAY_MCON, ARRAY_SW};

const char arrayTypeString[arrayTypes][32] =
{
    "Female Connector", "Male Connector", "Switch"
};

const char arrayReference[arrayTypes][8] =
{
    "X", "X", "SA"
};

enum ArrayReferenceType {SA, X};

const int arrayReferenceTypes[arrayTypes] =
{
    SA, SA, X
};

const char arrayOrientationString[2][32] =
{
    "Left", "Right"
};

#endif  // ARRAY_IMAGE_H
