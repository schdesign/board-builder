// schematic.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include "array.h"
#include "circuitsymbol.h"
#include "device.h"
#include "element.h"
#include "package.h"
#include "types.h"
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <QByteArray>

constexpr int windowSizeX = 1200;
constexpr int windowSizeY = 800;
constexpr int grid = 10;

const QString packagesDirectory = "../../../library/packages";
const QString packagesFile = "packages.lib";
const QString symbolsDirectory = "../../../library/symbols";
const QString symbolsFile = "symbols.lib";

class Pin
{
public:
    Pin() {}
    Pin(QString reference, int number, int x, int y, int net):
        reference(reference), number(number), x(x), y(y), net(net) {}

    QString reference;
    int number;     // pin number of device or element
    int x;
    int y;
    int net;        // net number
};

class Wire
{
public:
    Wire() {}
    Wire(int x1, int y1, int x2, int y2, int net, QString name = "", int nameSide = 0):
         x1(x1), y1(y1), x2(x2), y2(y2), net(net), name(name), nameSide(nameSide) {}

    int x1;
    int y1;
    int x2;
    int y2;
    int net;        // net number
    QString name;   // net name, for horizontal wire
    int nameSide;   // 0: left, 1: right
};

class Schematic
{
    typedef std::list<Wire> Net;
    typedef std::list<Wire>::iterator WireIt;

public:
    Schematic();
    void addArray(int type, int pins, int x, int y, int orientation);
    void addCircuitSymbol(int circuitSymbolType, int x, int y);
    void addDevice(int symbolNameID, int x, int y);
    void addElement(int elementType, int x, int y, int orientation);
    void addJunction(int x, int y);
    void addNet();
    void addNetName(int x, int y);
    void addPoint(int x, int y);
    void clear();
    void componentList(QString &text);
    bool connected(const Pin &pin, const Wire &wire);
    bool connected(const Wire &wire1, const Wire &wire2);
    void deleteElement(int x, int y);
    void deleteJunction(int x, int y);
    void deleteNet(int x, int y);
    void deleteWire(int x, int y);
    void draw(QPainter &painter);
    void enumerate();
    void errorCheck(QString &text);
    template<typename Type>
    void errorCheck(std::map<QString, QString> &components, Type t);
    void fromJson(const QByteArray &array);
    void horizontalMirror(int x, int y);
    bool insideConnected(int x, int y, const Wire &wire);
    bool insideConnected(const Pin &pin, const Wire &wire);
    bool joinLines(int &x11, int &x12, int &x21, int &x22);
    bool joinWires(Wire &wire1, Wire &wire2);
    template<typename Type>
    void makeComponentList(Type &t, const QString &key, const QString &value1,
                           const QString &value2);
    void move(int x, int y);
    void moveGroup();
    void moveGroup(int x, int y);
    void moveDown();
    void moveLeft();
    void moveRight();    
    void moveUp();
    QJsonObject netlist();
    template<typename Type>
    void netlist(QJsonArray &netlistElements, Type t, QString str);
    template<typename Type>
    int padNumber(const Type &t, int pinNumber);
    int padNumber(const Element &e, int pinNumber);
    void readFile(const QString &filename, QString &text);
    void readJsonFile(const QString &filename, QByteArray &byteArray);
    void readPackageLibrary(const QString &libraryname);
    void readPackages(const QByteArray &byteArray);
    void readSymbolLibrary(const QString &libraryname);
    void readSymbols(const QByteArray &byteArray);
    void reduceWires(std::list<Wire> &wires);
    void setNetNumber(int &net1, int &net2);
    void setValue(int x, int y);
    QJsonObject toJson();
    void updateNets();
    template<typename Type>
    void writeComponentList(const Type &t, QString &text);
    QJsonObject writePackageLibrary();
    QJsonObject writeSymbolLibrary();

    bool selectedArray;
    bool selectedCircuitSymbol;
    bool selectedDevice;
    bool selectedElement;
    bool selectedWire;
    bool showNetNumbers;
    int arrayNumber;
    int arrayOrientation;
    int deviceSymbolNameID;
    int diodeTypeID;
    int pointNumber;
    Array array;
    Device device;
    Element element;
    Net net;
    Point point;
    QRect groupBorder;
    QString key;
    QString value;
    Wire wire;
    std::list<Pin> pins;
    std::list<Wire> wires;
    std::map<int, Array> arrays;                  // array.center, array
    std::map<int, CircuitSymbol> circuitSymbols;  // circuitSymbol.center, circuitSymbol
    std::map<int, Device> devices;                // device.center, device
    std::map<int, Element> elements;              // element.center, element
    std::set<int> junctions;    // x > 0 (16 high bits), y > 0 (16 low bits)
    std::vector<Package> packages;
    std::vector<Point> points;
};

#endif  // SCHEMATIC_H
