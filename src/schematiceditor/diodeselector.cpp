// diodeselector.cpp
// Copyright (C) 2019 Alexander Karpeko

#include "diodeselector.h"

DiodeSelector::DiodeSelector(QString &title, QDialog *parent):
    QDialog(parent)
{
    setupUi(this);
    setGeometry(QRect(93, 100, 300, 220));
    setWindowTitle(title);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    selectorType = -1;
    if (title == "Dual Diode") selectorType = 0;
    if (title == "Dual Schottky Diode") selectorType = 1;
    if (title == "Dual Zener Diode") selectorType = 2;
}

void DiodeSelector::accept()
{
    constexpr int typeSize = 3;
    constexpr int types[typeSize][4] =
    {
        {COMMON_ANODE_DIODES, COMMON_CATHODE_DIODES, SERIES_DIODES, SINGLE_DIODE},
        {COMMON_ANODE_SCHOTTKY, COMMON_CATHODE_SCHOTTKY, SERIES_SCHOTTKY, SINGLE_SCHOTTKY},
        {COMMON_ANODE_ZENER, COMMON_CATHODE_ZENER, SERIES_ZENER, SINGLE_ZENER}
    };

    int type = -1;

    if (selectorType >= 0 && selectorType < typeSize) {
        if (commonAnodeRadioButton->isChecked()) type = types[selectorType][0];
        if (commonCathodeRadioButton->isChecked()) type = types[selectorType][1];
        if (connectedInSeriesRadioButton->isChecked()) type = types[selectorType][2];
        if (singleDiodeRadioButton->isChecked()) type = types[selectorType][3];
    }

    done(type + 1);
}
