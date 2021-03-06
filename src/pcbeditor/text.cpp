// text.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "board.h"
#include "text.h"
#include <cmath>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

void compareString(const QString &str, const QString &str2)
{
    if (str.compare(str2))
        throw ExceptionData(str2 + " error");
}

void Board::fromNetlist(const QByteArray &array)
{
    const int dx = 2000;
    const int dy = 5000;
    int x = dx;
    int y = 5 * dy;

    QJsonDocument document(QJsonDocument::fromJson(array));
    if (document.isNull())
        throw ExceptionData("Netlist file read error");

    QJsonObject object = document.object();

    clear();

    if (object["object"].toString() != "netlist")
        throw ExceptionData("File is not a netlist file");

    QJsonArray netlistElements(object["elements"].toArray());

    int size = netlistElements.size();
    int number = 2 * sqrt(size);
    for (int i = 0; i < size; i++) {
        Element element(netlistElements[i].toObject(), x, y);
        elements.push_back(element);
        if (i && !(i % number)) {
            x = dx;
            y += dy;
        }
        else
            x += dx;
    }

    getNets();
}

void Board::fromJson(const QByteArray &array)
{
    QJsonDocument document(QJsonDocument::fromJson(array));
    if (document.isNull())
        throw ExceptionData("Board file read error");

    QJsonObject object = document.object();

    clear();

    if (object["object"].toString() != "board")
        throw ExceptionData("File is not a board file");

    border.fromJson(object["borderPolygon"]);
    openMaskOnVia = object["openMaskOnVia"].toBool();
    Element::padCornerRadius = object["padCornerRadius"].toDouble();
    polygonSpace = object["polygonSpace"].toInt();
    solderMaskSwell = object["solderMaskSwell"].toInt();

    QJsonArray elementArray(object["elements"].toArray());
    QJsonArray topPolygonArray(object["topPolygons"].toArray());
    QJsonArray bottomPolygonArray(object["bottomPolygons"].toArray());
    QJsonArray topSegmentArray(object["topSegments"].toArray());
    QJsonArray bottomSegmentArray(object["bottomSegments"].toArray());
    QJsonArray viaArray(object["vias"].toArray());

    for (auto ea : elementArray) {
        Element e(ea.toObject());
        elements.push_back(e);
    }

    for (auto t : topPolygonArray) {
        Polygon p(t);
        topPolygons.push_back(p);
    }

    for (auto b : bottomPolygonArray) {
        Polygon p(b);
        bottomPolygons.push_back(p);
    }

    for (auto t : topSegmentArray) {
        Segment s(t);
        topSegments.push_back(s);
    }

    for (auto b : bottomSegmentArray) {
        Segment s(b);
        bottomSegments.push_back(s);
    }

    for (auto va : viaArray) {
        Via v(va);
        vias.push_back(v);
    }

    getNets();
}

void Board::readPackages(const QByteArray &byteArray)
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
        Element::addPackage(p);
}

QJsonObject Board::toJson()
{
    QJsonArray elementArray;
    for (auto e : elements)
        elementArray.append(e.toJson());

    QJsonArray topPolygonArray;
    for (auto t : topPolygons)
        topPolygonArray.append(t.toJson());

    QJsonArray bottomPolygonArray;
    for (auto b : bottomPolygons)
        bottomPolygonArray.append(b.toJson());

    QJsonArray topSegmentArray;
    for (auto t : topSegments)
        topSegmentArray.append(t.toJson());

    QJsonArray bottomSegmentArray;
    for (auto b : bottomSegments)
        bottomSegmentArray.append(b.toJson());

    QJsonArray viaArray;
    for (auto v : vias)
        viaArray.append(v.toJson());

    QJsonObject object
    {
        {"borderPolygon", border.toJson()},
        {"bottomPolygons", bottomPolygonArray},
        {"bottomSegments", bottomSegmentArray},
        {"elements", elementArray},
        {"object", "board"},
        {"openMaskOnVia", openMaskOnVia},
        {"padCornerRadius", Element::padCornerRadius},
        {"polygonSpace", polygonSpace},
        {"solderMaskSwell", solderMaskSwell},
        {"topPolygons", topPolygonArray},
        {"topSegments", topSegmentArray},
        {"vias", viaArray}
    };

    return object;
}

/********************************************************************************

void Board::componentList(QString &text)
{
    typedef std::map<QString, QString>::iterator ComponentIt;
    typedef std::multimap<QString, QString>::iterator ComponentIt2;

    std::map <QString, QString> components;         // reference, value
    std::multimap <QString, QString> components2;   // value, reference

    for (DeviceIt i = devices.begin(); i != devices.end(); ++i) {
        components.insert(std::make_pair((*i).second.reference, (*i).second.name));
        components2.insert(std::make_pair((*i).second.name, (*i).second.reference));
    }

    for (ElementIt i = elements.begin(); i != elements.end(); ++i) {
        components.insert(std::make_pair((*i).second.reference, (*i).second.value));
        components2.insert(std::make_pair((*i).second.value, (*i).second.reference));
    }

    text = "List by reference.\n";
    for (ComponentIt i = components.begin(); i != components.end(); ++i)
        text += (*i).first + "\t" + (*i).second + "\n";
    text += "\n";

    text += "List by value.\n";
    for (ComponentIt2 i = components2.begin(); i != components2.end(); ++i)
        text += (*i).first + "\t" + (*i).second + "\n";
}

void Board::errorCheck(QString &text)
{
    typedef std::map<QString, QString>::iterator ComponentIt;

    std::map <QString, QString> components; // reference pin, error

    for (DeviceIt i = devices.begin(); i != devices.end(); ++i)
        errorCheck(components, i);

    for (ElementIt i = elements.begin(); i != elements.end(); ++i)
        errorCheck(components, i);

    text = "Error check.\n";
    if (components.empty())
        text += "No errors.\n";
    else
        for (ComponentIt i = components.begin(); i != components.end(); ++i)
            text += (*i).first + "\t" + (*i).second + "\n";
}

template <class Type> void Board::errorCheck(std::map <QString,
                                   QString> &components, Type t)
{
    int netNumber;
    int x, y;
    QString str, str2, str3;

    for (uint i = 0; i < (*t).second.pins.size(); i++) {
        x = (*t).second.pins[i].x;
        y = (*t).second.pins[i].y;
        netNumber = -1;
        for (PinIt j = pins.begin(); j != pins.end(); ++j)
            if (x == (*j).x && y == (*j).y) {
                netNumber = (*j).net;
                break;
            }
        if (netNumber == -1) {
            str = (*t).second.reference + " pin: " + str2.setNum(i + 1);
            str3 = "error: not connected";
            components.insert(std::make_pair(str, str3));
        }
    }
}

void Board::netlist(QString &text)
{
    int size = devices.size() + elements.size();
    int space = 0;

    text = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    text += "<netlist>\n";
    setTextValue(text, ++space, "elements");
    setTextValue(text, ++space, "size", size);

    for (DeviceIt i = devices.begin(); i != devices.end(); ++i)
        netlist(text, space, i, (*i).second.name);

    for (ElementIt i = elements.begin(); i != elements.end(); ++i)
        netlist(text, space, i, (*i).second.value);

    setTextValue(text, --space, "/elements");
    text += "</netlist>\n";
}

template <class Type> void Board::netlist(QString &text,
           int &space, Type t, QString str)
{
    int netNumber;
    int x, y;

    setTextValue(text, space, "element");
    setTextValue(text, ++space, "reference", (*t).second.reference);
    setTextValue(text, space, "name", str);
    setTextValue(text, space, "package", (*t).second.package);

    setTextValue(text, space, "pads");
    setTextValue(text, ++space, "size", (*t).second.pins.size());
    for (uint i = 0; i < (*t).second.pins.size(); i++) {
        setTextValue(text, space, "pad");
        setTextValue(text, ++space, "number", i + 1);
        x = (*t).second.pins[i].x;
        y = (*t).second.pins[i].y;
        netNumber = -1;
        for (PinIt j = pins.begin(); j != pins.end(); ++j)
            if (x == (*j).x && y == (*j).y) {
                netNumber = (*j).net;
                break;
            }
        setTextValue(text, space, "net", netNumber);
        setTextValue(text, --space, "/pad");
    }
    setTextValue(text, --space, "/pads");

    setTextValue(text, --space, "/element");
}

************************************************************************/
