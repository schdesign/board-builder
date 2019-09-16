// schematic.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
#include "schematic.h"
#include "text.h"
#include <algorithm>
#include <cmath>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTextStream>

Schematic::Schematic()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    try {
        readPackageLibrary(packagesDirectory + "/" + packagesFile);
        readSymbolLibrary(symbolsDirectory + "/" + symbolsFile);
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(nullptr, QString("Error"), e.show());
    }

    selectedArray = false;
    selectedCircuitSymbol = false;
    selectedDevice = false;
    selectedElement = false;
    selectedWire = false;
    showNetNumbers = false;
}

void Schematic::addArray(int type, int number, int x, int y, int orientation)
{
    Array array(type, number, x, y, orientation);
    arrays[array.center] = array;
}

void Schematic::addCircuitSymbol(int circuitSymbolType, int x, int y)
{
    CircuitSymbol circuitSymbol(circuitSymbolType, x, y);
    circuitSymbols[circuitSymbol.center] = circuitSymbol;
}

void Schematic::addDevice(int nameID, int x, int y)
{
    Device device(nameID, x, y);
    devices[device.center] = device;
}

void Schematic::addElement(int elementType, int x, int y, int orientation)
{
    if (!elementType)
        return;

    Element element(elementType, x, y, orientation);
    elements[element.center] = element;
}

void Schematic::addJunction(int x, int y)
{
    int point = (x << 16) + y;
    junctions.insert(point);
}

void Schematic::addNet()
{    
    reduceWires(net);
    for (auto n : net)
        wires.push_back(n);
    net.clear();
    pointNumber = 0;
}

// Add net name for horizontal wire,
// name side is nearest to point: x, y
void Schematic::addNetName(int x, int y)
{
    static WireIt wireIt;

    if (!selectedWire)
        for (auto i = wires.begin(); i != wires.end(); ++i)
            if (y == (*i).y1 && (*i).y1 == (*i).y2 &&
                (insideConnected(x, y, *i) || (x == (*i).x1 || x == (*i).x2))) {
                deleteJunction(x, y);
                (*i).nameSide = 0;
                if (((*i).x2 > (*i).x1 && abs(x - (*i).x2) < abs(x - (*i).x1)) ||
                    ((*i).x1 > (*i).x2 && abs(x - (*i).x1) < abs(x - (*i).x2)))
                    (*i).nameSide = 1;
                wireIt = i;
                value.clear();
                selectedWire = true;
                return;
            }
    if (selectedWire) {
        (*wireIt).name = value;
        selectedWire = false;
    }
}

void Schematic::addPoint(int x, int y)
{
    if (pointNumber) {
        if (abs(x - point.x) >= abs(y - point.y))
            y = point.y;
        else
            x = point.x;
        net.push_back(Wire(point.x, point.y, x, y, -1));
    }

    point = Point(x, y);
    pointNumber++;
}

void Schematic::clear()
{
    arrays.clear();
    devices.clear();
    elements.clear();
    circuitSymbols.clear();
    pins.clear();
    wires.clear();
    junctions.clear();
}

bool Schematic::connected(const Pin &pin, const Wire &wire)
{
    if ((pin.x == wire.x1 && pin.y == wire.y1) ||
        (pin.x == wire.x2 && pin.y == wire.y2))
        return true;

    if (insideConnected(pin, wire))
        return true;

    return false;
}

bool Schematic::connected(const Wire &wire1, const Wire &wire2)
{
    int x, y;
    int point;

    // Wire ends connected
    if ((wire1.x1 == wire2.x1 && wire1.y1 == wire2.y1) ||
        (wire1.x1 == wire2.x2 && wire1.y1 == wire2.y2) ||
        (wire1.x2 == wire2.x1 && wire1.y2 == wire2.y1) ||
        (wire1.x2 == wire2.x2 && wire1.y2 == wire2.y2))
        return true;

    // Wire end connected with other wire
    if (insideConnected(wire1.x1, wire1.y1, wire2))
        return true;
    if (insideConnected(wire1.x2, wire1.y2, wire2))
        return true;
    if (insideConnected(wire2.x1, wire2.y1, wire1))
        return true;
    if (insideConnected(wire2.x2, wire2.y2, wire1))
        return true;

    // Wires crossed
    if (wire1.x1 == wire1.x2 && wire2.y1 == wire2.y2 &&
       ((wire1.x1 > wire2.x1 && wire1.x1 < wire2.x2) ||
        (wire1.x1 > wire2.x2 && wire1.x1 < wire2.x1)) &&
       ((wire2.y1 > wire1.y1 && wire2.y1 < wire1.y2) ||
        (wire2.y1 > wire1.y2 && wire2.y1 < wire1.y1))) {
        x = wire1.x1;
        y = wire2.y1;
        point = (x << 16) + y;
        if (junctions.count(point))
            return true;
    }
    if (wire1.y1 == wire1.y2 && wire2.x1 == wire2.x2 &&
       ((wire1.y1 > wire2.y1 && wire1.y1 < wire2.y2) ||
        (wire1.y1 > wire2.y2 && wire1.y1 < wire2.y1)) &&
       ((wire2.x1 > wire1.x1 && wire2.x1 < wire1.x2) ||
        (wire2.x1 > wire1.x2 && wire2.x1 < wire1.x1))) {
        x = wire2.x1;
        y = wire1.y1;
        point = (x << 16) + y;
        if (junctions.count(point))
            return true;
    }

    return false;
}

void Schematic::deleteElement(int x, int y)
{
    for (auto &a : arrays)
        if (a.second.exist(x, y)) {
            arrays.erase(a.first);
            return;
        }

    for (auto &c : circuitSymbols)
        if (c.second.exist(x, y)) {
            circuitSymbols.erase(c.first);
            return;
        }

    for (auto &d : devices)
        if (d.second.exist(x, y)) {
            devices.erase(d.first);
            return;
        }

    for (auto &e : elements)
        if (e.second.exist(x, y)) {
            elements.erase(e.first);
            return;
        }
}

void Schematic::deleteJunction(int x, int y)
{
    int point = (x << 16) + y;
    junctions.erase(point);
}

void Schematic::deleteNet(int x, int y)
{
    int netNumber = -1;

    for (auto i = wires.begin(); i != wires.end(); ++i)
        if (insideConnected(x, y, *i) ||
            (x == (*i).x1 && y == (*i).y1) ||
            (x == (*i).x2 && y == (*i).y2)) {
            netNumber = (*i).net;
            wires.erase(i);
            deleteJunction(x, y);
            break;
        }

    if (netNumber != -1)
        for (auto i = wires.end(); i != wires.begin();) {
            --i;
            if ((*i).net == netNumber)
                wires.erase(i);
        }
}

void Schematic::deleteWire(int x, int y)
{
    for (auto i = wires.begin(); i != wires.end(); ++i)
        if (insideConnected(x, y, *i) ||
            (x == (*i).x1 && y == (*i).y1) ||
            (x == (*i).x2 && y == (*i).y2)) {
            wires.erase(i);
            deleteJunction(x, y);
            return;
        }
}

void Schematic::draw(QPainter &painter)
{
    painter.setPen(QColor(200, 100, 100));
    QFont serifFont("Times", 10, QFont::Normal);
    painter.setFont(serifFont);

    // Draw arrays
    for (auto a : arrays)
        a.second.draw(painter);

    // Draw circuit symbols
    painter.setPen(QColor(200, 100, 100));
    for (auto c : circuitSymbols)
        c.second.draw(painter);

    // Draw devices
    for (auto d : devices)
        d.second.draw(painter);

    // Draw elements
    for (auto e : elements)
        e.second.draw(painter);

    // Draw group
    if (groupBorder.isValid()) {
        painter.setPen(QColor(200, 0, 0));
        painter.drawRect(groupBorder);
    }

    // Draw wires
    painter.setPen(QColor(0, 200, 0));
    for (auto w : wires) {
        painter.drawLine(w.x1, w.y1, w.x2, w.y2);
        if (!w.name.isEmpty()) {
            if (!w.nameSide)
                painter.drawText(w.x1, w.y1, w.name);
            else
                painter.drawText(w.x2 - 10 * w.name.size(), w.y1, w.name);
        }
    }
    for (auto n : net)
        painter.drawLine(n.x1, n.y1, n.x2, n.y2);

    // Draw junctions
    int k, x, y;
    for (auto j : junctions) {
        x = (j >> 16) & 0xffff;
        y = j & 0xffff;
        for (int i = 0; i < 5; i++) {
            k = 1;
            if (i > 0 && i < 4)
                k = 2;
            painter.drawLine(x-2+i, y-k, x-2+i, y+k);
        }
    }

    // Draw pin net numbers
    if (showNetNumbers) {
        QString str;
        painter.setPen(QColor(0, 200, 0));
        for (auto p : pins)
            painter.drawText(p.x+2, p.y, str.setNum(p.net));
    }
}

void Schematic::enumerate()
{
    int arrayCounter[arrayReferences] = {0};
    int deviceCounter[deviceReferences] = {0};
    int elementCounter[elementReferences] = {0};
    int number;
    QString str, str2;

    for (auto &a : arrays) {
        number = ++arrayCounter[a.second.referenceType];
        a.second.reference = arrayReference[a.second.type] + str.setNum(number);
    }

    for (auto &d : devices) {
        number = ++deviceCounter[d.second.referenceType];
        d.second.reference = deviceReference[d.second.type] + str.setNum(number);
        if (d.second.units.size() == 1)
            d.second.units[0].reference = d.second.reference;
        if (d.second.units.size() > 1)
            for (uint i = 0; i < d.second.units.size(); i++)
                d.second.units[i].reference = d.second.reference + "." + str2.setNum(i+1);
    }

    for (auto &e : elements) {
        number = ++elementCounter[e.second.referenceType];
        e.second.reference = elementReference[e.second.type] + str.setNum(number);
    }
}

// Junction insert if needed
bool Schematic::insideConnected(int x, int y, const Wire &wire)
{
    if ((wire.x1 == wire.x2 && x == wire.x1 &&
        ((y > wire.y1 && y < wire.y2) ||
         (y > wire.y2 && y < wire.y1))) ||
        (wire.y1 == wire.y2 && y == wire.y1 &&
        ((x > wire.x1 && x < wire.x2) ||
         (x > wire.x2 && x < wire.x1)))) {
        int point = (x << 16) + y;
        junctions.insert(point);
        return true;
    }

    return 0;
}

// Junction insert if needed
bool Schematic::insideConnected(const Pin &pin, const Wire &wire)
{
    if ((wire.x1 == wire.x2 && pin.x == wire.x1 &&
        ((pin.y > wire.y1 && pin.y < wire.y2) ||
         (pin.y > wire.y2 && pin.y < wire.y1))) ||
        (wire.y1 == wire.y2 && pin.y == wire.y1 &&
        ((pin.x > wire.x1 && pin.x < wire.x2) ||
         (pin.x > wire.x2 && pin.x < wire.x1)))) {
        int point = (pin.x << 16) + pin.y;
        junctions.insert(point);
        return true;
    }

    return 0;
}

bool Schematic::joinLines(int &x11, int &x12, int &x21, int &x22)
{
    int min1, max1, min2, max2;

    min1 = x11;
    max1 = x12;
    if (x12 < x11) {
        min1 = x12;
        max1 = x11;
    }
    min2 = x21;
    max2 = x22;
    if (x22 < x21) {
        min2 = x22;
        max2 = x21;
    }
    if (max1 >= min2 && max2 >= min1) {
        x11 = min1;
        if (min2 < min1)
            x11 = min2;
        x12 = max1;
        if (max2 > max1)
            x12 = max2;
        return true;
    }

    return false;
}

bool Schematic::joinWires(Wire &wire1, Wire &wire2)
{
    if (wire1.x1 == wire1.x2 && wire2.x1 == wire2.x2 && wire1.x1 == wire2.x1)
        return joinLines(wire1.y1, wire1.y2, wire2.y1, wire2.y2);

    if (wire1.y1 == wire1.y2 && wire2.y1 == wire2.y2 && wire1.y1 == wire2.y1)
        return joinLines(wire1.x1, wire1.x2, wire2.x1, wire2.x2);

    return false;
}

void Schematic::move(int x, int y)
{
    static int center;
    static int nameID;
    static int number;
    static int orientation;
    static int type;
    static int unitNumber;
    static QString value;
    static std::vector<QString> pinName;
    QString str;

    if (!selectedArray && !selectedCircuitSymbol &&
        !selectedDevice && !selectedElement) {
        for (auto a : arrays)
            if (a.second.exist(x, y)) {
                center = a.second.center;
                type = a.second.type;
                orientation = a.second.orientation;
                pinName = a.second.pinName;
                number = a.second.number;
                selectedArray = true;
                return;
            }
        for (auto c : circuitSymbols)
            if (c.second.exist(x, y)) {
                center = c.second.center;
                type = c.second.type;
                selectedCircuitSymbol = true;
                return;
            }
        for (auto d : devices) {
            unitNumber = d.second.exist(x, y);
            if (unitNumber > 0) {
                unitNumber--;
                center = d.second.center;
                nameID = d.second.nameID;
                selectedDevice = true;
                return;
            }
        }
        for (auto e : elements)
            if (e.second.exist(x, y)) {
                center = e.second.center;
                type = e.second.type;
                orientation = e.second.orientation;
                value = e.second.value;
                selectedElement = true;
                return;
            }
    }

    if (selectedArray) {
        arrays.erase(center);
        Array array(type, number, x, y, orientation);
        array.pinName = pinName;
        arrays[array.center] = array;
        selectedArray = false;
        return;
    }

    if (selectedCircuitSymbol) {
        circuitSymbols.erase(center);
        CircuitSymbol circuitSymbol(type, x, y);
        circuitSymbols[circuitSymbol.center] = circuitSymbol;
        selectedCircuitSymbol = false;
    }

    if (selectedDevice) {
        if (!unitNumber) {
            Device device(nameID, x, y);
            device.units.resize(1);
            for (uint i = 1; i < devices[center].units.size(); i++)
                device.units.push_back(devices[center].units[i]);
            devices.erase(center);
            center = device.center;
            devices[center] = device;
        }
        else {
            Unit unit(nameID, unitNumber, x, y);
            unit.reference = Device::symbols[nameID].reference +
                             "." + str.setNum(unitNumber+1);
            devices[center].units[unitNumber] = unit;
        }
        int n;
        for (uint i = 0; i < devices[center].pins.size(); i++) {
            n = devices[center].pins[i].unit - 1;
            devices[center].pins[i].x = devices[center].units[n].refX +
                                        Device::symbols[nameID].pins[i].x;
            devices[center].pins[i].y = devices[center].units[n].refY +
                                        Device::symbols[nameID].pins[i].y;
        }
        selectedDevice = false;
        return;
    }

    if (selectedElement) {
        elements.erase(center);
        Element element(type, x, y, orientation, value);
        elements[element.center] = element;
        selectedElement = false;
        return;
    }
}

void Schematic::moveGroup()
{
    int dx = points[2].x - points[0].x;
    int dy = points[2].y - points[0].y;
    int nameID;
    int orientation;
    int number;
    int type;    
    int x, y;
    int unitNumber;
    QString str;
    QString value;
    std::map<int, Array> arrays2;
    std::map<int, Device> devices2;
    std::map<int, Element> elements2;
    std::map<int, CircuitSymbol> circuitSymbols2;
    std::vector<int> centers;
    std::vector<int> unitNumbers;
    std::vector<QString> pinName;

    // Move arrays
    for (auto a : arrays)
        if (a.second.inside(points[0].x, points[0].y,
                            points[1].x, points[1].y)) {
            centers.push_back(a.second.center);
            type = a.second.type;
            x = a.second.refX + dx;
            y = a.second.refY + dy;
            orientation = a.second.orientation;
            pinName = a.second.pinName;
            number = a.second.number;
            Array array(type, number, x, y, orientation);
            array.pinName = pinName;
            arrays2[array.center] = array;
        }
    for (uint i = 0; i < centers.size(); i++)
        arrays.erase(centers[i]);
    centers.clear();
    for (auto c : arrays2)
        arrays[c.second.center] = c.second;
    arrays2.clear();

    // Move circuit symbols
    for (auto c : circuitSymbols)
        if (c.second.inside(points[0].x, points[0].y,
                            points[1].x, points[1].y)) {
            centers.push_back(c.second.center);
            type = c.second.type;
            x = c.second.refX + dx;
            y = c.second.refY + dy;
            CircuitSymbol circuitSymbol(type, x, y);
            circuitSymbols2[circuitSymbol.center] = circuitSymbol;
        }
    for (uint i = 0; i < centers.size(); i++)
        circuitSymbols.erase(centers[i]);
    centers.clear();
    for (auto c : circuitSymbols2)
        circuitSymbols[c.second.center] = c.second;
    circuitSymbols2.clear();

    // Move device units
    for (auto d : devices)
        if (d.second.inside(points[0].x, points[0].y,
                            points[1].x, points[1].y, unitNumbers)) {
            centers.push_back(d.second.center);
            nameID = d.second.nameID;
            x = d.second.refX;
            y = d.second.refY;
            if (!unitNumbers[0]) {
                x += dx;
                y += dy;
            }
            Device device(nameID, x, y);
            for (uint j = 0; j < unitNumbers.size(); j++) {
                unitNumber = unitNumbers[j];
                if (!unitNumber) {
                    device.units.resize(1);
                    for (uint k = 1; k < d.second.units.size(); k++)
                        device.units.push_back(d.second.units[k]);
                }
                if (unitNumber) {
                    x = d.second.units[unitNumber].refX + dx;
                    y = d.second.units[unitNumber].refY + dy;
                    Unit unit(nameID, unitNumber, x, y);
                    unit.reference = Device::symbols[nameID].reference +
                                     "." + str.setNum(unitNumber+1);
                    device.units[unitNumber] = unit;
                }
            }
            devices2[device.center] = device;
        }
    for (uint i = 0; i < centers.size(); i++)
        devices.erase(centers[i]);
    centers.clear();
    for (auto d : devices2)
        devices[d.second.center] = d.second;
    devices2.clear();

    // Move elements
    for (auto e : elements)
        if (e.second.inside(points[0].x, points[0].y,
                            points[1].x, points[1].y)) {
            centers.push_back(e.second.center);
            type = e.second.type;
            x = e.second.refX + dx;
            y = e.second.refY + dy;
            orientation = e.second.orientation;
            value = e.second.value;
            Element element(type, x, y, orientation, value);
            elements2[element.center] = element;
        }
    for (uint i = 0; i < centers.size(); i++)
        elements.erase(centers[i]);
    centers.clear();
    for (auto e : elements2)
        elements[e.second.center] = e.second;
    elements2.clear();
}

void Schematic::moveGroup(int x, int y)
{
    Point point(x, y);
    points.push_back(point);

    switch (points.size()) {
    case 1:
        return;
    case 2:
        groupBorder.setCoords(points[0].x, points[0].y,
                              points[1].x-1, points[1].y-1);
        if (groupBorder.isValid())
            return;
        break;
    case 3:
        moveGroup();
    }

    groupBorder.setCoords(1, 1, 0, 0);
    points.clear();
}

void Schematic::moveDown()
{
    //centerY += 0.1 * windowSizeY / scale;
}

void Schematic::moveLeft()
{
    //centerX -= 0.1 * windowSizeY / scale;
}

void Schematic::moveRight()
{
    //centerX += 0.1 * windowSizeY / scale;
}

void Schematic::moveUp()
{
    //centerY -= 0.1 * windowSizeY / scale;  // equal step for x and y
}

void Schematic::readFile(const QString &filename, QString &text)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw ExceptionData(filename + " open error");
    QTextStream in(&file);
    text = in.readAll();
    file.close();
}

void Schematic::readJsonFile(const QString &filename, QByteArray &byteArray)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw ExceptionData(filename + " open error");
    byteArray = file.readAll();
    file.close();
}

void Schematic::readPackageLibrary(const QString &libraryname)
{
    QByteArray byteArray;
    readJsonFile(libraryname, byteArray);

    QJsonDocument document(QJsonDocument::fromJson(byteArray));
    if (document.isNull())
        throw ExceptionData("Package library file read error");

    QJsonObject object = document.object();

    if (object["object"].toString() != "packageLibrary")
        throw ExceptionData("File is not a package library file");

    QJsonArray filenames(object["filenames"].toArray());

    for (auto f : filenames) {
        readJsonFile(packagesDirectory + "/" + f.toString(), byteArray);
        readPackages(byteArray);
    }
}

void Schematic::readSymbolLibrary(const QString &libraryname)
{
    QByteArray byteArray;
    readJsonFile(libraryname, byteArray);

    QJsonDocument document(QJsonDocument::fromJson(byteArray));
    if (document.isNull())
        throw ExceptionData("Shema file read error");

    QJsonObject object = document.object();

    if (object["object"].toString() != "symbolLibrary")
        throw ExceptionData("File is not a symbol library file");

    QJsonArray filenames(object["filenames"].toArray());

    for (auto f : filenames) {
        readJsonFile(symbolsDirectory + "/" + f.toString(), byteArray);
        readSymbols(byteArray);
    }
}

// Reduce number of wires
void Schematic::reduceWires(std::list <Wire> &wires)
{
    WireIt i, j;

    i = wires.end();
    while (i != wires.begin()) {
        --i;
        j = wires.end();
        --j;
        while (j != i) {
            if (joinWires(*i, *j))
                wires.erase(j);
            --j;
        }
    }
}

void Schematic::setNetNumber(int &net1, int &net2)
{
    if (net1 == -1) {
        net1 = net2;
        return;
    }
    if (net2 == -1) {
        net2 = net1;
        return;
    }
    if (net1 > net2)
        net1 = net2;
    else
        net2 = net1;
}

void Schematic::setValue(int x, int y)
{
    static int center;
    static int number;

    if (!selectedArray) {
        for (auto a : arrays)
            if (a.second.exist(x, y)) {
                center = a.second.center;
                number = (y - a.second.refY + 0.5 * a.second.deltaY) / a.second.deltaY;
                limit(number, 0, a.second.number);
                value.clear();
                selectedArray = true;
                return;
            }
    }
    if (selectedArray) {
        if (number == arrays[center].number)
            arrays[center].name = value;
        else
            arrays[center].pinName[number] = value;
        selectedArray = false;
        return;
    }

    if (!selectedElement) {
        for (auto e : elements)
            if (e.second.exist(x, y)) {
                center = e.second.center;
                value.clear();
                selectedElement = true;
                return;
            }
    }
    if (selectedElement) {
        elements[center].value = value;
        selectedElement = false;
        return;
    }
}

// Update nets and insert junctions if needed
void Schematic::updateNets()
{
    int groundNet = -1;
    int groundIecNet = -1;
    int maxGroundNet = 0;

    // Set groundIecNet
    for (auto c : circuitSymbols) {
        if (c.second.type == GROUND)
            groundNet = 0;
        if (c.second.type == GROUND_IEC)
            groundIecNet = 0;
    }

    if (!groundNet && !groundIecNet) {
        groundIecNet = 1;
        maxGroundNet = 1;
    }

    pins.clear();

    // Get all pins of arrays
    for (auto a : arrays)
        for (uint i = 0; i < a.second.pins.size(); ++i)
            pins.push_back(Pin(a.second.reference, i + 1,
                a.second.pins[i].x, a.second.pins[i].y, -1));

    // Get all pins of devices
    for (auto d : devices)
        for (uint i = 0; i < d.second.pins.size(); ++i)
            pins.push_back(Pin(d.second.reference, i + 1,
                d.second.pins[i].x, d.second.pins[i].y, -1));

    // Get all pins of elements
    for (auto e : elements)
        for (uint j = 0; j < e.second.pins.size(); ++j)
            pins.push_back(Pin(e.second.reference, j + 1,
                e.second.pins[j].x, e.second.pins[j].y, -1));

    // Get all pins of ground
    for (auto c : circuitSymbols) {
        int n = -1;
        if (c.second.type == GROUND)
            n = groundNet;
        if (c.second.type == GROUND_IEC)
            n = groundIecNet;
        if (n == -1)
            continue;
        pins.push_back(Pin(circuitSymbolTypeString[c.second.type], 1,
                           c.second.refX, c.second.refY, n));
    }

    reduceWires(wires);

    for (auto &w : wires)
        w.net = -1;

    // Add junctions
    for (auto i = pins.begin(); i != pins.end(); ++i) {
        int connect = 0;
        for (auto j = i; j != pins.end();) {
            ++j;
            if ((*i).x == (*j).x && (*i).y == (*j).y)
                connect++;
        }
        for (auto w : wires) {
            if (((*i).x == w.x1 && (*i).y == w.y1) ||
                ((*i).x == w.x2 && (*i).y == w.y2))
                connect++;
        }
        if (connect > 1)
            addJunction((*i).x, (*i).y);
    }

    // Current state:
    // ground pins: net = 0 and may be 1, other pins: net = -1
    // wires: net = -1

    bool finished = false;
    int netNumber = 0;
    int tmpNumber = -2;
    int unconnectedNumber = -3;

    while (!finished) {
        int connect = 1;
        auto tmp = pins.begin();
        for (auto i = pins.begin(); i != pins.end(); ++i) {
            if (netNumber <= maxGroundNet)
                if ((*i).net != netNumber)
                    continue;
            if (netNumber > maxGroundNet) {
                if ((*i).net != -1)
                    continue;
                (*i).net = netNumber;
                tmp = i;
                connect = 0;
            }

            // Set tmpNumber for pin to pin connection
            for (auto j = pins.begin(); j != pins.end(); ++j) {
                if (i == j || (*j).net != -1)
                    continue;
                if ((*i).x == (*j).x && (*i).y == (*j).y) {
                    (*j).net = tmpNumber;
                    connect = 1;
                }
            }

            // Set netNumber for pin to wire connection
            for (auto &w : wires)
                if (connected(*i, w) && w.net == -1)
                    w.net = netNumber;

            if (netNumber > maxGroundNet)
                break;
        }

        // Change pin net with tmpNumber to netNumber
        for (auto i = pins.begin(); i != pins.end(); ++i)
            if ((*i).net == tmpNumber)
                (*i).net = netNumber;

        // Set net number for wire to wire connection
        bool wireUpdated = true;
        while (wireUpdated) {
            wireUpdated = false;
            for (auto j = wires.begin(); j != wires.end(); ++j) {
                if ((*j).net != netNumber)
                    continue;
                for (auto k = wires.begin(); k != wires.end(); ++k) {
                    if (j == k || (*k).net != -1)
                        continue;
                    if (connected(*j, *k)) {
                        (*k).net = netNumber;
                        wireUpdated = true;
                    }
                }
            }
        }

        // Set net number for wire to pin connection
        for (auto w : wires) {
            if (w.net != netNumber)
                continue;
            for (auto i = pins.begin(); i != pins.end(); ++i)
                if (connected(*i, w) && (*i).net == -1) {
                    (*i).net = netNumber;
                    connect = 1;
                }
        }

        if (netNumber <= maxGroundNet || connect)
            netNumber++;

        if (netNumber > maxGroundNet && !connect)
            (*tmp).net = unconnectedNumber;

        finished = true;
        for (auto i = pins.begin(); i != pins.end(); ++i)
            if ((*i).net == -1) {
                finished = false;
                break;
            }
    }

    // Set net = -1 for unconnected pins
    for (auto i = pins.begin(); i != pins.end(); ++i)
        if ((*i).net == unconnectedNumber)
            (*i).net = -1;
}
