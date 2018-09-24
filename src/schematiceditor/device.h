// device.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef DEVICE_H
#define DEVICE_H

#include "unit.h"
#include <QJsonObject>
#include <QJsonValue>

class DevicePin
{
public:
    DevicePin() {}
    DevicePin(int x, int y, QString name, bool side, int unit):
        x(x), y(y), name(name), side(side), unit(unit) {}
    DevicePin(const QJsonValue &value);
    QJsonObject toJson();

    int x;
    int y;
    QString name;
    bool side;  // 0: left, 1: right
    int unit;   // unit number
};

class DeviceSymbol
{
public:
    QJsonObject toJson();

    bool showPinName;
    int nameID;
    int nameTextX;
    int nameTextY;
    int pinNameXLeft;
    int pinNameXRight;
    int pinNameY;
    int type;
    int unitsNumber;
    QString name;
    QString package;
    QString reference;
    Unit unit;
    std::vector<DevicePin> pins;
};

class Device
{
public:
    Device() {}
    Device(int nameID, int refX, int refY);
    Device(const QJsonObject &object);
    static void addSymbol(const QJsonValue &value);
    static QJsonObject writeSymbols();
    void draw(QPainter &painter);
    int exist(int x, int y);
    void init();
    bool inside(int leftX, int topY, int rightX, int bottomY,
                std::vector<int> &unitNumbers);
    QJsonObject toJson();

    static int symbolID;
    static std::map <int, DeviceSymbol> symbols;  // device nameID, deviceSymbol
    bool showPinName;
    int center;         // device center is center of 1st unit
    int nameID;
    int nameTextX;      // left bottom point of text
    int nameTextY;
    int pinNameXLeft;
    int pinNameXRight;
    int pinNameY;
    int referenceType;  // D
    int refX;           // point of top left pin of 1st unit
    int refY;
    int type;
    int unitsNumber;
    QString name;
    QString package;
    QString reference;  // D1
    Unit unit;
    std::vector<DevicePin> pins;
    std::vector<Unit> units;
};

#endif  // DEVICE_H
