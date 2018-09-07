// schema.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
#include "schema.h"
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

Schema::Schema()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    try {
        readPackageLibrary(packagesDirectory + "/" + packagesFile);
        readSymbolLibrary(symbolsDirectory + "/" + symbolsFile);
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(NULL, QString("Error"), e.show());
    }

    selectedArray = false;
    selectedDevice = false;
    selectedElement = false;
    selectedSymbol = false;
    selectedWire = false;
    showNetNumbers = false;
}

void Schema::addArray(int type, int number, int x, int y, int orientation)
{
    Array array(type, number, x, y, orientation);
    arrays[array.center] = array;
}

void Schema::addDevice(int nameID, int x, int y)
{
    Device device(nameID, x, y);
    devices[device.center] = device;
}

void Schema::addElement(int elementType, int x, int y, int orientation)
{
    if (!elementType)
        return;

    Element element(elementType, x, y, orientation);
    elements[element.center] = element;
}

void Schema::addJunction(int x, int y)
{
    int point = (x << 16) + y;
    junctions.insert(point);
}

void Schema::addNet()
{    
    reduceWires(net);
    for (auto n : net)
        wires.push_back(n);
    net.clear();
    pointNumber = 0;
}

// Add net name for horizontal wire,
// name side is nearest to point: x, y
void Schema::addNetName(int x, int y)
{
    static WireIt wireIt;

    if (!selectedWire)
        for (auto i = wires.begin(); i != wires.end(); ++i)
            if (y == (*i).y1 && (*i).y1 == (*i).y2 &&
                (insideConnected(x, y, *i) || (x == (*i).x1 || x == (*i).x2))) {
                deleteJunction(x, y);
                (*i).nameSide = 0;
                if (((*i).x2 > (*i).x1 && fabs(x - (*i).x2) < fabs(x - (*i).x1)) ||
                    ((*i).x1 > (*i).x2 && fabs(x - (*i).x1) < fabs(x - (*i).x2)))
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

void Schema::addPoint(int x, int y)
{
    if (pointNumber) {
        if (fabs(x - point.x) >= fabs(y - point.y))
            y = point.y;
        else
            x = point.x;
        net.push_back(Wire(point.x, point.y, x, y, -1));
    }

    point = Point(x, y);
    pointNumber++;
}

void Schema::addSymbol(int symbolType, int x, int y)
{
    Symbol symbol(symbolType, x, y);
    symbols[symbol.center] = symbol;
}

void Schema::clear()
{
    arrays.clear();
    devices.clear();
    elements.clear();
    symbols.clear();
    pins.clear();
    wires.clear();
    junctions.clear();
}

bool Schema::connected(const Pin &pin, const Wire &wire)
{
    if ((pin.x == wire.x1 && pin.y == wire.y1) ||
        (pin.x == wire.x2 && pin.y == wire.y2))
        return true;

    if (insideConnected(pin, wire))
        return true;

    return false;
}

bool Schema::connected(const Wire &wire1, const Wire &wire2)
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

void Schema::deleteElement(int x, int y)
{
    for (auto &a : arrays)
        if (a.second.exist(x, y)) {
            arrays.erase(a.first);
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

    for (auto &s : symbols)
        if (s.second.exist(x, y)) {
            symbols.erase(s.first);
            return;
        }
}

void Schema::deleteJunction(int x, int y)
{
    int point = (x << 16) + y;
    junctions.erase(point);
}

void Schema::deleteNet(int x, int y)
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

void Schema::deleteWire(int x, int y)
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

void Schema::draw(QPainter &painter)
{
    painter.setPen(QColor(200, 100, 100));
    QFont serifFont("Times", 10, QFont::Normal);
    painter.setFont(serifFont);

    // Draw arrays
    for (auto a : arrays)
        a.second.draw(painter);

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

    // Draw symbols
    painter.setPen(QColor(200, 100, 100));
    for (auto s : symbols)
        s.second.draw(painter);

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

void Schema::enumerate()
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
bool Schema::insideConnected(int x, int y, const Wire &wire)
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
bool Schema::insideConnected(const Pin &pin, const Wire &wire)
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

bool Schema::joinLines(int &x11, int &x12, int &x21, int &x22)
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

bool Schema::joinWires(Wire &wire1, Wire &wire2)
{
    if (wire1.x1 == wire1.x2 && wire2.x1 == wire2.x2 && wire1.x1 == wire2.x1)
        return joinLines(wire1.y1, wire1.y2, wire2.y1, wire2.y2);

    if (wire1.y1 == wire1.y2 && wire2.y1 == wire2.y2 && wire1.y1 == wire2.y1)
        return joinLines(wire1.x1, wire1.x2, wire2.x1, wire2.x2);

    return false;
}

void Schema::move(int x, int y)
{
    static int center;
    static int nameID;
    static int number;
    static int orientation;
    static int pinNumber;
    static int type;
    static int unitNumber;
    static QString value;
    static std::vector<QString> pinName;
    QString str;

    if (!selectedArray && !selectedDevice &&
        !selectedElement && !selectedSymbol) {
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
        for (auto d : devices)
            if (unitNumber == d.second.exist(x, y)) {
                unitNumber--;
                center = d.second.center;
                nameID = d.second.nameID;
                selectedDevice = true;
                return;
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
        for (auto s : symbols)
            if (s.second.exist(x, y)) {
                center = s.second.center;
                type = s.second.type;
                selectedSymbol = true;
                return;
            }
    }

    if (selectedArray) {
        arrays.erase(center);
        Array array(type, pinNumber, x, y, orientation);
        array.pinName = pinName;
        arrays[array.center] = array;
        selectedArray = false;
        return;
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

    if (selectedSymbol) {
        symbols.erase(center);
        Symbol symbol(type, x, y);
        symbols[symbol.center] = symbol;
        selectedSymbol = false;
    }
}

void Schema::moveGroup()
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
    std::map<int, Symbol> symbols2;
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

    // Move symbols
    for (auto s : symbols)
        if (s.second.inside(points[0].x, points[0].y,
                            points[1].x, points[1].y)) {
            centers.push_back(s.second.center);
            type = s.second.type;
            x = s.second.refX + dx;
            y = s.second.refY + dy;
            Symbol symbol(type, x, y);
            symbols2[symbol.center] = symbol;
        }
    for (uint i = 0; i < centers.size(); i++)
        symbols.erase(centers[i]);
    centers.clear();
    for (auto s : symbols2)
        symbols[s.second.center] = s.second;
    symbols2.clear();
}

void Schema::moveGroup(int x, int y)
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

void Schema::moveDown()
{
    //centerY += 0.1 * windowSizeY / scale;
}

void Schema::moveLeft()
{
    //centerX -= 0.1 * windowSizeY / scale;
}

void Schema::moveRight()
{
    //centerX += 0.1 * windowSizeY / scale;
}

void Schema::moveUp()
{
    //centerY -= 0.1 * windowSizeY / scale;  // equal step for x and y
}

void Schema::readFile(const QString &filename, QString &text)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw ExceptionData(filename + " open error");
    QTextStream in(&file);
    text = in.readAll();
    file.close();
}

void Schema::readJsonFile(const QString &filename, QByteArray &byteArray)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw ExceptionData(filename + " open error");
    byteArray = file.readAll();
    file.close();
}

void Schema::readPackageLibrary(const QString &libraryname)
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

void Schema::readSymbolLibrary(const QString &libraryname)
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
void Schema::reduceWires(std::list <Wire> &wires)
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

void Schema::setNetNumber(int &net1, int &net2)
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

void Schema::setValue(int x, int y)
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
void Schema::updateNets()
{
    const int maxLength = 10;
    int connect;

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
    for (auto s : symbols)
        pins.push_back(Pin(symbolTypeString[s.second.type], 1,
            s.second.refX, s.second.refY, 0));

    reduceWires(wires);

    for (auto &w : wires)
        w.net = -1;

    // Add junctions
    for (auto i = pins.begin(); i != pins.end(); ++i) {
        connect = 0;
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

    int netNumber = 0;
    int delta = 0;

    for (auto i = pins.begin(); i != pins.end(); ++i) {

        if ((*i).net > 0)
            continue;

        delta = 1;

        // Set net number for pin to pin connection
        for (auto j = i; j != pins.end();) {
            ++j;
            if ((*i).x == (*j).x && (*i).y == (*j).y) {
                if ((*i).net == -1 && (*j).net == -1) {
                    (*i).net = netNumber + delta;
                    delta = 0;
                    (*j).net = (*i).net;
                    continue;
                }
                setNetNumber((*i).net, (*j).net);
            }
        }

        // Set net number for pin to wire connection
        for (auto &w : wires)
            if (connected(*i, w)) {
                if ((*i).net == -1 && w.net == -1) {
                    (*i).net = netNumber + delta;
                    delta = 0;
                    w.net = (*i).net;
                    continue;
                }
                setNetNumber((*i).net, w.net);
            }

        // Set net number for wire to wire connection
        for (int l = 0; l < maxLength; l++)
            for (auto j = wires.begin(); j != wires.end(); ++j)
                for (auto k = wires.begin(); k != wires.end(); ++k)
                    if (j != k && connected(*j, *k))
                        setNetNumber((*j).net, (*k).net);

        // Set net number for wire to pin connection
        for (auto w : wires)
            for (auto k = i; k != pins.end();) {
                ++k;
                if (connected(*k, w))
                    setNetNumber(w.net, (*k).net);
            }

        if (!delta)
            netNumber++;
    }
}
