// device.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "device.h"
#include "exceptiondata.h"
#include "function.h"
#include "text.h"
#include <QJsonArray>

int Device::symbolID = 0;
std::map<int, DeviceSymbol> Device::symbols;

DevicePin::DevicePin(const QJsonValue &value)
{
    QJsonObject object = value.toObject();

    x = object["x"].toInt();
    y = object["y"].toInt();
    name = object["name"].toString();
    side = object["side"].toBool();
    unit = object["unit"].toInt();
}

QJsonObject DevicePin::toJson()
{
    QJsonObject object
    {
        {"x", x},
        {"y", y},
        {"name", name},
        {"side", side},
        {"unit", unit}
    };

    return object;
}

QJsonObject DeviceSymbol::toJson()
{
    QString typeString(deviceTypeString[type]);

    QJsonArray devicePins;
    for (auto p : pins)
        devicePins.append(p.toJson());

    QJsonArray deviceUnits;
    for (int i = 0; i < unitsNumber; i++)
          deviceUnits.append(Unit::symbols[(nameID<<8)+i].toJson());

    QJsonObject object
    {
        {"showPinName", showPinName},
        {"pinNameXLeft", pinNameXLeft},
        {"pinNameXRight", pinNameXRight},
        {"pinNameY", pinNameY},
        {"type", typeString},
        {"unitsNumber", unitsNumber},
        {"name", name},
        {"package", packageName},
        {"pins", devicePins},
        {"units", deviceUnits}
    };

    return object;
}

Device::Device(int symbolNameID, int refX, int refY):
    symbolNameID(symbolNameID), refX(refX), refY(refY)
{
    init();
}

Device::Device(const QString &name, int symbolNameID, int refX, int refY):
    symbolNameID(symbolNameID), refX(refX), refY(refY), name(name)
{
    init();
}

Device::Device(const QJsonObject &object)
{
    name = object["name"].toString();
    packageName = object["package"].toString();
    reference = object["reference"].toString();
    symbolName = object["symbolName"].toString();
    QJsonArray deviceUnits(object["units"].toArray());
    refX = deviceUnits[0].toObject()["refX"].toInt();
    refY = deviceUnits[0].toObject()["refY"].toInt();

    for (int i = 0; i < Device::symbolID; i++) {
        if (!symbolName.compare(Device::symbols[i].name)) {
            symbolNameID = Device::symbols[i].nameID;
            break;
        }
        if (i == Device::symbolID - 1)
            throw ExceptionData("Device name error");
    }

    init();
    units.clear();

    for (int i = 0; i < unitsNumber; i++) {
        Unit unit(symbolNameID, deviceUnits[i].toObject());
        units.push_back(unit);
    }

    for (uint i = 0; i < pins.size(); i++) {
        int n = pins[i].unit - 1;
        pins[i].x = units[n].refX + symbols[symbolNameID].pins[i].x;
        pins[i].y = units[n].refY + symbols[symbolNameID].pins[i].y;
    }
}

void Device::addSymbol(const QJsonValue &value)
{
    DeviceSymbol symbol;
    QJsonObject object = value.toObject();

    symbol.nameID = Device::symbolID;

    symbol.showPinName = object["showPinName"].toBool();

    symbol.pinNameXLeft = object["pinNameXLeft"].toInt();
    symbol.pinNameXRight = object["pinNameXRight"].toInt();
    symbol.pinNameY = object["pinNameY"].toInt();

    QString typeString(object["type"].toString());
    if (!findIndex(symbol.type, typeString, deviceTypeString, deviceTypes))
        throw ExceptionData("Array type error");
    symbol.reference = deviceReference[symbol.type];

    symbol.name = object["name"].toString();
    symbol.packageName = object["package"].toString();

    QJsonArray devicePins = object["pins"].toArray();

    for (auto d : devicePins) {
        DevicePin pin(d);
        symbol.pins.push_back(pin);
    }

    QJsonArray unitArray = object["units"].toArray();
    symbol.unitsNumber = unitArray.size();

    for (int i = 0; i < symbol.unitsNumber; i++)
        Unit::addSymbol(unitArray[i], symbol.nameID);

    Device::symbols[symbol.nameID] = symbol;
    Device::symbolID++;
}

void Device::draw(QPainter &painter, int fontSize)
{
    int dx;
    int h, w;
    int x, y;
    QString str;

    for (auto u : units)
        u.draw(painter, fontSize);

    for (uint i = 0; i < pins.size(); i++) {
        x = pins[i].x;
        y = pins[i].y;
        dx = 0;
        if (i > 8 || pins[i].side)
            dx = -5;
        painter.drawText(x + dx, y, str.setNum(i+1));
        if (showPinName) {
            int h = 10;
            int w = h * pins[i].name.size();
            if (pins[i].side)
                painter.drawText(x + pinNameXRight - w, y + pinNameY - 5, w, h,
                                 Qt::AlignRight | Qt::AlignVCenter, pins[i].name);
            else
                painter.drawText(x + pinNameXLeft, y + pinNameY - 5, w, h,
                                 Qt::AlignLeft | Qt::AlignVCenter, pins[i].name);
        }
    }

    w = fontSize * name.size();
    h = fontSize;
    x = units[0].centerX - w / 2;
    y = units[0].border.bottomY + 1;
    painter.drawText(x, y, w, h, Qt::AlignCenter, name);
}

int Device::exist(int x, int y)
{
    for (uint i = 0; i < units.size(); i++)
        if (units[i].exist(x, y))
            return i+1;

    return 0;
}

void Device::init()
{
    const DeviceSymbol &symbol = symbols[symbolNameID];
    int id;
    int x, y;
    QString str;

    showPinName = symbol.showPinName;
    pinNameXLeft = symbol.pinNameXLeft;
    pinNameXRight = symbol.pinNameXRight;
    pinNameY = symbol.pinNameY;

    type = symbol.type;
    referenceType = deviceReferenceTypes[type];
    unitsNumber = symbol.unitsNumber;

    if (name.isEmpty())
        name = symbol.name;
    if (packageName.isEmpty())
        packageName = symbol.packageName;
    if (reference.isEmpty())
        reference = symbol.reference;

    symbolName = symbol.name;

    DevicePin pin;
    for (auto sp : symbol.pins) {
        pin.name = sp.name;
        pin.unit = sp.unit;
        pin.side = sp.side;
        id = (symbolNameID << 8) + pin.unit - 1;
        pin.x = refX + Unit::symbols[id].refX + sp.x;
        pin.y = refY + Unit::symbols[id].refY + sp.y;
        pins.push_back(pin);
    }

    for (int i = 0; i < symbol.unitsNumber; i++) {
        id = (symbolNameID << 8) + i;
        x = refX + Unit::symbols[id].refX;
        y = refY + Unit::symbols[id].refY;
        Unit unit(symbolNameID, i, x, y);
        unit.reference = reference;
        if (symbol.unitsNumber > 1)
            unit.reference += "." + str.setNum(i+1);
        units.push_back(unit);
    }

    center = units[0].center;
}

bool Device::inside(int leftX, int topY, int rightX, int bottomY,
                    std::vector<int> &unitNumbers)
{
    unitNumbers.clear();

    for (uint i = 0; i < units.size(); i++)
        if (units[i].inside(leftX, topY, rightX, bottomY))
            unitNumbers.push_back(i);

    return unitNumbers.size();
}

QJsonObject Device::toJson()
{
    QJsonArray deviceUnits;
    for (auto u : units)
        deviceUnits.append(u.toJson());

    QJsonObject object
    {
        {"reference", reference},
        {"name", name},
        {"package", packageName},
        {"symbolName", symbolName},
        {"units", deviceUnits}
    };

    return object;
}

QJsonObject Device::writeSymbols()
{
    QJsonArray deviceSymbols;
    for (auto d : Device::symbols)
        deviceSymbols.append(d.second.toJson());

    QJsonObject object
    {
        {"devices", deviceSymbols},
        {"object", "symbols"}
    };

    return object;
}
