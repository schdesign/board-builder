// unitimage.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef UNIT_IMAGE_H
#define UNIT_IMAGE_H

constexpr int deviceTypes = 3;
constexpr int deviceReferences = 3;

enum DeviceType {ANALOG_DEVICE, DIGITAL_DEVICE, MIXED_DEVICE};

const char deviceTypeString[deviceTypes][32] =
{
    "Analog device", "Digital device", "Mixed device"
};

enum DeviceReferenceType {DA, DD, D};

constexpr int deviceReferenceTypes[deviceTypes] = {DA, DD, D};

const char deviceReference[deviceTypes][8] = {"DA", "DD", "D"};

#endif  // UNIT_IMAGE_H
