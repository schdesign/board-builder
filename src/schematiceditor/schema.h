// schema.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SCHEMA_H
#define SCHEMA_H

#include "array.h"
#include "device.h"
#include "element.h"
#include "symbol.h"
#include "types.h"
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <QByteArray>

const int windowSizeX = 1200;
const int windowSizeY = 800;
const int grid = 10;

const QString packagesDirectory = "library/packages";
const QString packagesFile = "packages.lib";
const QString symbolsDirectory = "library/symbols";
const QString symbolsFile = "symbols.lib";

class Package
{
public:
    int pins;
    QString name;
};

class Pin
{
public:
    QString reference;
    int number;     // pin number of device or element
    int x;
    int y;
    int net;        // net number

    Pin() {}
    Pin(QString _reference, int _number, int _x, int _y, int _net):
        reference(_reference), number(_number), x(_x), y(_y), net(_net) {}
    ~Pin() {}
};

class Wire
{
public:
    int x1;
    int y1;
    int x2;
    int y2;
    int net;        // net number
    QString name;   // net name, for horizontal wire
    int nameSide;   // 0: left, 1: right

    Wire() {}
    Wire(int _x1, int _y1, int _x2, int _y2, int _net, QString _name = "", int _nameSide = 0):
         x1(_x1), y1(_y1), x2(_x2), y2(_y2), net(_net), name(_name), nameSide(_nameSide) {}
    ~Wire() {}
};

typedef std::list<Wire> Net;
typedef std::list<Wire>::iterator WireIt;
typedef std::vector<Package> Packages;

class Schema
{
public:
    std::map<int, Array> arrays;        // array.center, array
    std::map<int, Device> devices;      // device.center, device
    std::map<int, Element> elements;    // element.center, element
    std::map<int, Symbol> symbols;      // symbol.center, symbol
    std::list<Pin> pins;
    std::list<Wire> wires;
    std::set<int> junctions;    // x > 0 (16 high bits), y > 0 (16 low bits)
    Array array;
    Device device;
    Element element;
    Packages packages;
    Wire wire;
    Net net;
    Point point;
    std::vector<Point> points;
    int pointNumber;
    bool selectedArray;
    bool selectedDevice;
    bool selectedElement;
    bool selectedSymbol;
    bool selectedWire;
    bool showNetNumbers;
    QRect groupBorder;
    QString key;
    QString value;
    int arrayOrientation;
    int arrayNumber;
    int deviceNameID;

    Schema();
    ~Schema() {}
    void addArray(int type, int pins, int x, int y, int orientation);
    void addDevice(int nameID, int x, int y);
    void addElement(int elementType, int x, int y, int orientation);
    void addJunction(int x, int y);
    void addNet();
    void addNetName(int x, int y);
    void addPackage(const QJsonValue &value);
    void addPoint(int x, int y);
    void addSymbol(int symbolType, int x, int y);
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
    template <class Type>
    void errorCheck(std::map <QString, QString> &components, Type t);
    void fromJson(const QByteArray &array);
    bool insideConnected(int x, int y, const Wire &wire);
    bool insideConnected(const Pin &pin, const Wire &wire);
    bool joinLines(int &x11, int &x12, int &x21, int &x22);
    bool joinWires(Wire &wire1, Wire &wire2);
    void move(int x, int y);
    void moveGroup();
    void moveGroup(int x, int y);
    void moveDown();
    void moveLeft();
    void moveRight();    
    void moveUp();
    QJsonObject netlist();
    template <class Type>
    void netlist(QJsonArray &netlistElements, Type t, QString str);
    void readFile(const QString &filename, QString &text);
    void readJsonFile(const QString &filename, QByteArray &byteArray);
    void readPackageLibrary(const QString &libraryname);
    void readPackages(const QByteArray &byteArray);
    void readSymbolLibrary(const QString &libraryname);
    void readSymbols(const QByteArray &byteArray);
    void reduceWires(std::list <Wire> &wires);
    void setNetNumber(int &net1, int &net2);
    void setValue(int x, int y);
    QJsonObject toJson();
    void updateNets();
    QJsonObject writePackageLibrary();
    QJsonObject writeSymbolLibrary();
};

#endif  // SCHEMA_H
