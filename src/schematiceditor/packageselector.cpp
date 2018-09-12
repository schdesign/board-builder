// packageselector.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "packageselector.h"
#include <QtGlobal>

PackageSelector::PackageSelector(Schematic &schematic, QDialog *parent):
    QDialog(parent), schematic(schematic)
{
    setupUi(this);
    setGeometry(QRect(93, 100, 600, 500));

    for (auto sa : schematic.arrays) {
        elementNames.push_back(sa.second.reference + "  " + sa.second.name + "  ");
        packageNames.push_back(sa.second.package);
        elementPins.push_back(sa.second.pins.size());
    }
    for (auto sd : schematic.devices) {
        elementNames.push_back(sd.second.reference + "  " + sd.second.name + "  ");
        packageNames.push_back(sd.second.package);
        elementPins.push_back(sd.second.pins.size());
    }
    for (auto se : schematic.elements) {
        elementNames.push_back(se.second.reference + "  " + se.second.value + "  ");
        packageNames.push_back(se.second.package);
        elementPins.push_back(se.second.pins.size());
    }

    for (uint row = 0; row < elementNames.size(); row++)
        elementListWidget->insertItem(row, elementNames[row] + packageNames[row]);
    elementListWidget->setCurrentRow(0);

    for (uint row = 0; row < schematic.packages.size(); row++)
        packageListWidget->insertItem(row, schematic.packages[row].name);
    packageListWidget->setCurrentRow(0);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearPackage()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectPackage()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void PackageSelector::accept()
{
    int row = 0;

    for (auto &sa : schematic.arrays)
        sa.second.package = packageNames[row++];
    for (auto &sd : schematic.devices)
        sd.second.package = packageNames[row++];
    for (auto &se : schematic.elements)
        se.second.package = packageNames[row++];

    done(1);
}

void PackageSelector::clearPackage()
{
    int row = elementListWidget->currentRow();
    packageNames[row].clear();
    elementListWidget->takeItem(row);
    elementListWidget->insertItem(row, elementNames[row]);
    elementListWidget->setCurrentRow(row);
}

void PackageSelector::connectPackage()
{
    int row = elementListWidget->currentRow();
    int row2 = packageListWidget->currentRow();
    if (elementPins[row] != schematic.packages[row2].pins)
        return;
    packageNames[row] = schematic.packages[row2].name;
    elementListWidget->takeItem(row);
    elementListWidget->insertItem(row, elementNames[row] + packageNames[row]);
    elementListWidget->setCurrentRow(row);
}
