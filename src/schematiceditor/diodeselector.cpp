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
    if (title == "Double Diode")
        selectorType = 0;
    if (title == "Double Schottky Diode")
        selectorType = 1;
}

void DiodeSelector::accept()
{
    int type = -1;

    if (selectorType == 0) {
        if (singleDiodeRadioButton->isChecked()) type = SINGLE_DIODE;
        if (commonAnodeRadioButton->isChecked()) type = COMMON_ANODE_DIODES;
        if (commonCathodeRadioButton->isChecked()) type = COMMON_CATHODE_DIODES;
        if (connectedInSeriesRadioButton->isChecked()) type = SERIES_DIODES;
    }

    if (selectorType == 1) {
        if (singleDiodeRadioButton->isChecked()) type = SINGLE_SCHOTTKY;
        if (commonAnodeRadioButton->isChecked()) type = COMMON_ANODE_SCHOTTKY;
        if (commonCathodeRadioButton->isChecked()) type = COMMON_CATHODE_SCHOTTKY;
        if (connectedInSeriesRadioButton->isChecked()) type = SERIES_SCHOTTKY;
    }

    done(type + 1);
}
