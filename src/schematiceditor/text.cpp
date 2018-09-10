// text.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "schema.h"
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

void Schema::addPackage(const QJsonValue &value)
{
    Package package;

    QJsonObject object = value.toObject();

    package.name = object["name"].toString();

    QJsonArray padsArray = object["pads"].toArray();
    package.pins = padsArray.size();

    packages.push_back(package);
}

void Schema::componentList(QString &text)
{
    std::map <QString, QString> components;         // reference, value
    std::multimap <QString, QString> components2;   // value, reference

    for (auto a : arrays) {
        if (a.second.name.isEmpty())
            continue;
        components.insert(std::make_pair(a.second.reference, a.second.name));
        components2.insert(std::make_pair(a.second.name, a.second.reference));
    }

    for (auto d : devices) {
        components.insert(std::make_pair(d.second.reference, d.second.name));
        components2.insert(std::make_pair(d.second.name, d.second.reference));
    }

    for (auto e : elements) {
        components.insert(std::make_pair(e.second.reference, e.second.value));
        components2.insert(std::make_pair(e.second.value, e.second.reference));
    }

    text = "List by reference.\n";
    for (auto c : components)
        text += c.first + "\t" + c.second + "\n";
    text += "\n";

    text += "List by value.\n";
    for (auto c : components2)
        text += c.first + "\t" + c.second + "\n";
}

void Schema::errorCheck(QString &text)
{
    std::map<QString, QString> components; // reference pin, error

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

template <class Type> void Schema::errorCheck(std::map<QString, QString> &components, Type t)
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

void Schema::fromJson(const QByteArray &array)
{
    QJsonDocument document(QJsonDocument::fromJson(array));
    if (document.isNull())
        throw ExceptionData("Shema file read error");

    QJsonObject object = document.object();

    clear();

    if (object["object"].toString() != "schema")
        throw ExceptionData("File is not a shema file");

    QJsonArray schemaArrays(object["arrays"].toArray());
    QJsonArray schemaDevices(object["devices"].toArray());
    QJsonArray schemaElements(object["elements"].toArray());
    QJsonArray schemaSymbols(object["symbols"].toArray());
    QJsonArray schemaWires(object["wires"].toArray());
    QJsonArray schemaJunctions(object["junctions"].toArray());

    for (auto s : schemaArrays) {
        Array array(s.toObject());
        arrays[array.center] = array;
    }

    for (auto s : schemaDevices) {
        Device device(s.toObject());
        devices[device.center] = device;
    }

    for (auto s : schemaElements) {
        Element element(s.toObject());
        elements[element.center] = element;
    }

    for (auto s : schemaSymbols) {
        Symbol symbol(s.toObject());
        symbols[symbol.center] = symbol;
    }

    for (auto s : schemaWires) {
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

    for (auto s : schemaJunctions) {
        int x = s.toObject()["x"].toInt();
        int y = s.toObject()["y"].toInt();
        addJunction(x, y);
    }

    updateNets();
}

QJsonObject Schema::netlist()
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

template <class Type>
void Schema::netlist(QJsonArray &netlistElements, Type t, QString str)
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

void Schema::readPackages(const QByteArray &byteArray)
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

void Schema::readSymbols(const QByteArray &byteArray)
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

QJsonObject Schema::toJson()
{
    QJsonArray schemaArrays;
    for (auto a : arrays)
        schemaArrays.append(a.second.toJson());

    QJsonArray schemaDevices;
    for (auto d : devices)
        schemaDevices.append(d.second.toJson());

    QJsonArray schemaElements;
    for (auto e : elements)
        schemaElements.append(e.second.toJson());

    QJsonArray schemaSymbols;
    for (auto s : symbols)
        schemaSymbols.append(s.second.toJson());

    QJsonArray schemaWires;
    for (auto w : wires) {
        QJsonObject schemaWire
        {
            {"x1", w.x1},
            {"y1", w.y1},
            {"x2", w.x2},
            {"y2", w.y2},
            {"net", w.net},
            {"name", w.name},
            {"nameSide", w.nameSide}
        };
        schemaWires.append(schemaWire);
    }

    QJsonArray schemaJunctions;
    for (auto j : junctions) {
        QJsonObject schemaJunction
        {
            {"x", (j >> 16) & 0xffff},
            {"y", j & 0xffff}
        };
        schemaJunctions.append(schemaJunction);
    }

    QJsonObject object
    {
        {"object", "schema"},
        {"arrays", schemaArrays},
        {"devices", schemaDevices},
        {"elements", schemaElements},
        {"symbols", schemaSymbols},
        {"wires", schemaWires},
        {"junctions", schemaJunctions}
    };

    return object;
}
