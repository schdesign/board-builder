// packageselector.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "packageselector.h"
#include <algorithm>
#include <QtGlobal>

PackageSelector::PackageSelector(Schematic &schematic, QDialog *parent):
    QDialog(parent), schematic(schematic)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 780, 500));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearPackage()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectPackage()));
    connect(elementListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(itemSelectionChanged()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(mapButton, SIGNAL(clicked()), this, SLOT(mapPads()));

    pad2NumbersComboBox->hide();
    pad3NumbersComboBox->hide();

    for (auto s : schematic.arrays) {
        centers.push_back(s.first);
        names.push_back(s.second.reference + "  " + s.second.name + "  ");
        packageNames.push_back(s.second.packageName);
        pins.push_back(s.second.pins.size());
    }
    for (auto s : schematic.devices) {
        centers.push_back(s.first);
        names.push_back(s.second.reference + "  " + s.second.name + "  ");
        packageNames.push_back(s.second.packageName);
        pins.push_back(s.second.pins.size());
    }
    firstElementRow = centers.size();
    for (auto s : schematic.elements) {
        centers.push_back(s.first);
        names.push_back(s.second.reference + "  " + s.second.value + "  ");
        packageNames.push_back(s.second.packageName);
        padsMaps.push_back(s.second.padsMap);
        pins.push_back(s.second.pins.size());
    }
    pastLastElementRow = centers.size();

    for (uint row = 0; row < names.size(); row++)
        elementListWidget->insertItem(row, names[row] + packageNames[row]);
    elementListWidget->setCurrentRow(0);

    for (uint row = 0; row < schematic.packages.size(); row++)
        packageListWidget->insertItem(row, schematic.packages[row].name);
    packageListWidget->setCurrentRow(0);
}

void PackageSelector::accept()
{
    int row = 0;

    for (auto &s : schematic.arrays)
        s.second.packageName = packageNames[row++];
    for (auto &s : schematic.devices)
        s.second.packageName = packageNames[row++];
    for (auto &s : schematic.elements) {
        s.second.packageName = packageNames[row];
        s.second.padsMap = padsMaps[row++];
    }

    done(1);
}

void PackageSelector::clearPackage()
{
    int row = elementListWidget->currentRow();
    packageNames[row].clear();
    if (row >= firstElementRow && row < pastLastElementRow) {
        const Element &element = schematic.elements[centers[row]];
        bool isEqualPinsType = false;
        for (auto e : equalPinsTypes)
            if (element.type == e) {
                isEqualPinsType = true;
                break;
            }
        if (!isEqualPinsType)
            padsMaps[row] = 0;
    }
    elementListWidget->takeItem(row);
    elementListWidget->insertItem(row, names[row]);
    elementListWidget->setCurrentRow(row);
}

void PackageSelector::connectPackage()
{
    int row = elementListWidget->currentRow();
    int row2 = packageListWidget->currentRow();
    if (pins[row] != schematic.packages[row2].pads.size())
        return;
    packageNames[row] = schematic.packages[row2].name;
    elementListWidget->takeItem(row);
    QString str = names[row] + packageNames[row];
    if (row >= firstElementRow && row < pastLastElementRow)
        str += "  " + QString::number(schematic.elements[centers[row]].padsMap);
    elementListWidget->insertItem(row, str);
    elementListWidget->setCurrentRow(row);
}

void PackageSelector::drawElement(QPainter &painter, int row)
{
    painter.setPen(QColor(200, 100, 100));

    const Element &e = schematic.elements[centers[row]];
    int orientation = e.orientation;
    QString packageName = e.packageName;
    int padsMap = e.padsMap;
    int type = e.type;
    QString value = e.value;
    int dx = e.centerX - e.refX;
    int dy = e.centerY - e.refY;
    int x = symbolWindowX + symbolWindowWidth / 2 - dx;
    int y = symbolWindowY + symbolWindowHeight / 2 - dy;
    Element element(type, x, y, orientation, value);
    element.packageName = packageName;
    element.padsMap = padsMap;

    element.draw(painter, false, true);
}

void PackageSelector::drawPackage(QPainter &painter, int row)
{
    double length;
    double scale;
    int d, h, inD, w;
    int dx, dy;
    int index;
    int rx, ry;
    int x, y;
    QString packageName = packageNames[row];
    QString str;

    if (schematic.packages.size() == 0)
        return;

    for (index = 0; index < schematic.packages.size(); index++)
        if (!packageName.compare(schematic.packages[index].name))
            break;

    if (index == schematic.packages.size())
        return;

    const Package &package = schematic.packages[index];

    length = 0.6 * (packageWindowWidth < packageWindowHeight ?
             packageWindowWidth : packageWindowHeight);

    scalePackage(package, length, scale);

    dx = packageWindowX + packageWindowWidth / 2;
    dy = packageWindowY + packageWindowHeight / 2;
    dx -= scale * package.outerBorderCenterX;
    dy -= scale * package.outerBorderCenterY;

    // Draw pads
    painter.setPen(QColor(255, 0, 0));
    for (auto p : package.pads) {
        d = scale * p.diameter;
        h = scale * p.height;
        inD = scale * p.innerDiameter;
        w = scale * p.width;
        if (p.orientation == RIGHT) {
            h = scale * p.width;
            w = scale * p.height;
        }
        if (h == 0 || w == 0) {
            h = d;
            w = d;
        }
        x = scale * p.x - w / 2 + dx;
        y = scale * p.y - h / 2 + dy;
        rx = d / 2;
        ry = rx;
        QPainterPath path;
        path.addRoundedRect(x, y, w, h, rx, ry);
        painter.setBrush(QColor(255, 255, 255));
        painter.drawPath(path);
        painter.drawText(x, y, w, h, Qt::AlignCenter, str.setNum(p.number));
    }

    // Draw package
    painter.setPen(QColor(0, 150, 0));
    for (auto e : package.ellipses)
        painter.drawEllipse(scale * (e.x - e.w / 2) + dx,
                            scale * (e.y - e.h / 2) + dy,
                            scale * e.w, scale * e.h);
    for (auto l : package.lines)
        painter.drawLine(scale * l.x1 + dx, scale * l.y1 + dy,
                         scale * l.x2 + dx, scale * l.y2 + dy);
}

void PackageSelector::itemSelectionChanged()
{
    update();

    pinNumbersLabel->setText("");
    pad2NumbersComboBox->hide();
    pad3NumbersComboBox->hide();
    mapButton->setEnabled(false);

    int row = elementListWidget->currentRow();
    if (row < firstElementRow || row >= pastLastElementRow)
        return;

    const Element &element = schematic.elements[centers[row]];

    for (auto e : equalPinsTypes)
        if (element.type == e)
            return;

    if (element.pins.size() == 2)
        pinNumbersLabel->setText("1   2");

    if (element.pins.size() == 3)
        pinNumbersLabel->setText("1   2   3");

    if (schematic.packages.size() == 0)
        return;

    int index;
    QString packageName = packageNames[row];

    for (index = 0; index < schematic.packages.size(); index++)
        if (!packageName.compare(schematic.packages[index].name))
            break;

    if (index == schematic.packages.size())
        return;

    const Package &package = schematic.packages[index];

    if (element.pins.size() != package.pads.size())
        return;

    if (package.pads.size() == 2) {
        pad2NumbersComboBox->show();
        mapButton->setEnabled(true);
    }

    if (package.pads.size() == 3) {
        pad3NumbersComboBox->show();
        mapButton->setEnabled(true);
    }
}

void PackageSelector::mapPads()
{
    int row = elementListWidget->currentRow();
    if (row < firstElementRow || row >= pastLastElementRow)
        return;

    const Element &element = schematic.elements[centers[row]];

    for (auto e : equalPinsTypes)
        if (element.type == e)
            return;

    if (schematic.packages.size() == 0)
        return;

    int index;
    QString packageName = packageNames[row];

    for (index = 0; index < schematic.packages.size(); index++)
        if (!packageName.compare(schematic.packages[index].name))
            break;

    if (index == schematic.packages.size())
        return;

    const Package &package = schematic.packages[index];

    if (element.pins.size() != package.pads.size())
        return;

    QString text;

    if (package.pads.size() == 2) {
        text = pad2NumbersComboBox->currentText();
        padsMaps[row] = padsMapFromString(text, 2);
    }

    if (package.pads.size() == 3) {
        text = pad3NumbersComboBox->currentText();
        padsMaps[row] = padsMapFromString(text, 3);
    }
}

int PackageSelector::padsMapFromString(const QString str, int size)
{
    int padsMap = 0;

    if (size == 2) {

    }

    if (size == 3) {

    }

    return padsMap;
}

void PackageSelector::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont serifFont("Times", 11, QFont::Normal);
    QString str;

    painter.setPen(QColor(200, 200, 200));
    painter.setFont(serifFont);

    QRect symbolWindow(symbolWindowX, symbolWindowY,
                       symbolWindowWidth, symbolWindowHeight);
    QRect packageWindow(packageWindowX, packageWindowY,
                        packageWindowWidth, packageWindowHeight);
    painter.fillRect(symbolWindow, QColor(255, 255, 255));
    painter.fillRect(packageWindow, QColor(255, 255, 255));
    painter.drawRect(symbolWindow);
    painter.drawRect(packageWindow);

    int row = elementListWidget->currentRow();
    if (row < firstElementRow || row >= pastLastElementRow)
        return;

    drawElement(painter, row);
    drawPackage(painter, row);
}

void PackageSelector::scalePackage(const Package &package, double length, double &scale)
{
    int width = package.outerBorder.rightX - package.outerBorder.leftX;
    int height = package.outerBorder.bottomY - package.outerBorder.topY;
    int maxLength = std::max(width, height);

    scale = length / maxLength;
}
