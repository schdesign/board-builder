// packageselector.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "packageselector.h"
#include <QtGlobal>

PackageSelector::PackageSelector(Schema &schema, QDialog *parent):
    QDialog(parent), schema(schema)
{
    setupUi(this);
    setGeometry(QRect(93, 100, 600, 500));

    for (auto sa : schema.arrays) {
        elementNames.push_back(sa.second.reference + "  " + sa.second.name + "  ");
        packageNames.push_back(sa.second.package);
        elementPins.push_back(sa.second.pins.size());
    }
    for (auto sd : schema.devices) {
        elementNames.push_back(sd.second.reference + "  " + sd.second.name + "  ");
        packageNames.push_back(sd.second.package);
        elementPins.push_back(sd.second.pins.size());
    }
    for (auto se : schema.elements) {
        elementNames.push_back(se.second.reference + "  " + se.second.value + "  ");
        packageNames.push_back(se.second.package);
        elementPins.push_back(se.second.pins.size());
    }

    for (uint row = 0; row < elementNames.size(); row++)
        elementListWidget->insertItem(row, elementNames[row] + packageNames[row]);
    elementListWidget->setCurrentRow(0);

    for (uint row = 0; row < schema.packages.size(); row++)
        packageListWidget->insertItem(row, schema.packages[row].name);
    packageListWidget->setCurrentRow(0);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clearPackage()));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(connectPackage()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void PackageSelector::accept()
{
    int row = 0;

    for (auto &sa : schema.arrays)
        sa.second.package = packageNames[row++];
    for (auto &sd : schema.devices)
        sd.second.package = packageNames[row++];
    for (auto &se : schema.elements)
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
    if (elementPins[row] != schema.packages[row2].pins)
        return;
    packageNames[row] = schema.packages[row2].name;
    elementListWidget->takeItem(row);
    elementListWidget->insertItem(row, elementNames[row] + packageNames[row]);
    elementListWidget->setCurrentRow(row);
}
