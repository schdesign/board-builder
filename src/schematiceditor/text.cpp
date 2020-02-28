// text.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
#include "schematic.h"
#include "text.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

void compareString(const QString &str, const QString &str2)
{
    if (str.compare(str2))
        throw ExceptionData(str2 + " error");
}

void Schematic::addPackage(const QJsonValue &value)
{
    Package package;

    QJsonObject object = value.toObject();

    package.name = object["name"].toString();

    QJsonArray padsArray = object["pads"].toArray();
    package.pins = padsArray.size();

    packages.push_back(package);
}

void Schematic::componentList(QString &text)
{
    // Reference; value, package
    std::map<QString, std::vector<QString>, LessReference> components;
    // Value; package, reference
    std::multimap<QString, std::vector<QString>> components2;

    for (auto a : arrays) {
        auto &c = a.second;
        if (c.name.isEmpty())
            continue;
        makeComponentList(components, c.reference, c.name, c.package);
        makeComponentList(components2, c.name, c.package, c.reference);
    }

    for (auto d : devices) {
        auto &c = d.second;
        makeComponentList(components, c.reference, c.name, c.package);
        makeComponentList(components2, c.name, c.package, c.reference);
    }

    for (auto e : elements) {
        auto &c = e.second;
        makeComponentList(components, c.reference, c.value, c.package);
        makeComponentList(components2, c.value, c.package, c.reference);
    }

    int maxSize = 0;
    int maxSize2 = 0;
    for (auto c : components) {
        if (c.first.size() > maxSize)
            maxSize = c.first.size();
        if (c.second[0].size() > maxSize2)
            maxSize2 = c.second[0].size();
    }

    text = "Component list by reference.\n";
    writeComponentList(components, text);
    text += "\n";
    text += "Component list by value.\n";
    writeComponentList(components2, text);
}

void Schematic::errorCheck(QString &text)
{
    std::map<QString, QString> components;  // reference pin, error

    for (auto i = arrays.begin(); i != arrays.end(); ++i)
        errorCheck(components, i);

    for (auto i = devices.begin(); i != devices.end(); ++i)
        errorCheck(components, i);

    for (auto i = elements.begin(); i != elements.end(); ++i)
        errorCheck(components, i);

    text = "Error check.\n";
    if (components.empty())
        text += "No errors.\n";
    else
        for (auto c : components)
            text += c.first + "\t" + c.second + "\n";
}

template <typename Type>
void Schematic::errorCheck(std::map<QString, QString> &components, Type t)
{
    int netNumber;
    int x, y;
    QString str, str2, str3;

    for (uint i = 0; i < (*t).second.pins.size(); i++) {
        x = (*t).second.pins[i].x;
        y = (*t).second.pins[i].y;
        netNumber = -1;
        for (auto p : pins)
            if (x == p.x && y == p.y) {
                netNumber = p.net;
                break;
            }
        if (netNumber == -1) {
            str = (*t).second.reference + " pin: " + str2.setNum(i + 1);
            str3 = "error: not connected";
            components.insert(std::make_pair(str, str3));
        }
    }
}

void Schematic::fromJson(const QByteArray &array)
{
    QJsonDocument document(QJsonDocument::fromJson(array));
    if (document.isNull())
        throw ExceptionData("Shematic file read error");

    QJsonObject object = document.object();

    clear();

    if (object["object"].toString() != "schematic")
        throw ExceptionData("File is not a shematic file");

    QJsonArray schematicArrays(object["arrays"].toArray());
    QJsonArray schematicDevices(object["devices"].toArray());
    QJsonArray schematicElements(object["elements"].toArray());
    QJsonArray schematicCircuitSymbols(object["circuitSymbols"].toArray());
    QJsonArray schematicWires(object["wires"].toArray());
    QJsonArray schematicJunctions(object["junctions"].toArray());

    for (auto s : schematicArrays) {
        Array array(s.toObject());
        arrays[array.center] = array;
    }

    for (auto c : schematicCircuitSymbols) {
        CircuitSymbol circuitSymbol(c.toObject());
        circuitSymbols[circuitSymbol.center] = circuitSymbol;
    }

    for (auto s : schematicDevices) {
        Device device(s.toObject());
        devices[device.center] = device;
    }

    for (auto s : schematicElements) {
        Element element(s.toObject());
        elements[element.center] = element;
    }

    for (auto s : schematicWires) {
        QJsonObject obj(s.toObject());
        Wire wire;
        wire.x1 = obj["x1"].toInt();
        wire.y1 = obj["y1"].toInt();
        wire.x2 = obj["x2"].toInt();
        wire.y2 = obj["y2"].toInt();
        wire.net = obj["net"].toInt();
        wire.name = obj["name"].toString();
        wire.nameSide = obj["nameSide"].toInt();
        wires.push_back(wire);
    }

    for (auto s : schematicJunctions) {
        int x = s.toObject()["x"].toInt();
        int y = s.toObject()["y"].toInt();
        addJunction(x, y);
    }

    updateNets();
}

template<typename Type>
void Schematic::makeComponentList(Type &t, const QString &key, const QString &value1,
                                  const QString &value2)
{
    static std::vector<QString> str(2);
    str[0] = value1;
    str[1] = value2;
    t.insert(std::make_pair(key, str));
}

QJsonObject Schematic::netlist()
{
    QJsonArray netlistElements;

    for (auto a : arrays)
        netlist(netlistElements, a.second, a.second.name);

    for (auto d : devices)
        netlist(netlistElements, d.second, d.second.name);

    for (auto e : elements)
        netlist(netlistElements, e.second, e.second.value);

    QJsonObject object
    {
        {"object", "netlist"},
        {"elements", netlistElements}
    };

    return object;
}

template <typename Type>
void Schematic::netlist(QJsonArray &netlistElements, Type t, QString str)
{
    QJsonArray elementPads;

    for (uint i = 0; i < t.pins.size(); i++) {
        int x = t.pins[i].x;
        int y = t.pins[i].y;
        int netNumber = -1;
        for (auto p : pins)
            if (x == p.x && y == p.y) {
                netNumber = p.net;
                break;
            }

        QJsonObject elementPad
        {
            {"number", int(i + 1)},
            {"net", netNumber}
        };

        elementPads.append(elementPad);
    }

    QJsonObject object
    {
        {"reference", t.reference},
        {"name", str},
        {"package", t.package},
        {"pads", elementPads}
    };

    netlistElements.append(object);
}

void Schematic::readPackages(const QByteArray &byteArray)
{
    QJsonParseError error;
    QString str;

    QJsonDocument document(QJsonDocument::fromJson(byteArray, &error));
    if (document.isNull())
        throw ExceptionData("Packages file read error: " + error.errorString() +
                            ", offset: " + str.setNum(error.offset));

    QJsonObject object = document.object();

    if (object["object"].toString() != "packages")
        throw ExceptionData("File is not a packages file");

    QJsonArray packageArray(object["packages"].toArray());
    for (auto p : packageArray)
        addPackage(p);
}

void Schematic::readSymbols(const QByteArray &byteArray)
{
    QJsonDocument document(QJsonDocument::fromJson(byteArray));
    if (document.isNull())
        throw ExceptionData("Symbol file read error");

    QJsonObject object = document.object();

    if (object["object"].toString() != "symbols")
        throw ExceptionData("File is not a symbol file");

    if (object.contains("arrays")) {
        QJsonArray arraySymbols(object["arrays"].toArray());
        for (auto a : arraySymbols)
            Array::addSymbol(a);
         return;
    }

    if (object.contains("devices")) {
        QJsonArray deviceSymbols(object["devices"].toArray());
        for (auto d : deviceSymbols)
            Device::addSymbol(d);
        return;
    }

    if (object.contains("elements")) {
        QJsonArray elementSymbols(object["elements"].toArray());
        for (auto e : elementSymbols)
            Element::addSymbol(e);
        return;
    }
}

QJsonObject Schematic::toJson()
{
    QJsonArray schematicArrays;
    for (auto a : arrays)
        schematicArrays.append(a.second.toJson());

    QJsonArray schematicCircuitSymbols;
    for (auto c : circuitSymbols)
        schematicCircuitSymbols.append(c.second.toJson());

    QJsonArray schematicDevices;
    for (auto d : devices)
        schematicDevices.append(d.second.toJson());

    QJsonArray schematicElements;
    for (auto e : elements)
        schematicElements.append(e.second.toJson());

    QJsonArray schematicWires;
    for (auto w : wires) {
        QJsonObject schematicWire
        {
            {"x1", w.x1},
            {"y1", w.y1},
            {"x2", w.x2},
            {"y2", w.y2},
            {"net", w.net},
            {"name", w.name},
            {"nameSide", w.nameSide}
        };
        schematicWires.append(schematicWire);
    }

    QJsonArray schematicJunctions;
    for (auto j : junctions) {
        QJsonObject schematicJunction
        {
            {"x", (j >> 16) & 0xffff},
            {"y", j & 0xffff}
        };
        schematicJunctions.append(schematicJunction);
    }

    QJsonObject object
    {
        {"object", "schematic"},
        {"arrays", schematicArrays},
        {"circuitSymbols", schematicCircuitSymbols},
        {"devices", schematicDevices},
        {"elements", schematicElements},
        {"wires", schematicWires},
        {"junctions", schematicJunctions}
    };

    return object;
}

template<typename Type>
void Schematic::writeComponentList(const Type &t, QString &text)
{
    int maxSize = 0;
    int maxSize2 = 0;

    for (auto c : t) {
        if (c.first.size() > maxSize)
            maxSize = c.first.size();
        if (c.second[0].size() > maxSize2)
            maxSize2 = c.second[0].size();
    }

    for (auto c : t)
        text += c.first.leftJustified(maxSize + 2, ' ') +
                c.second[0].leftJustified(maxSize2 + 2, ' ') + c.second[1] + "\n";
}
