// diodeselector.cpp
// Copyright (C) 2019 Alexander Karpeko

#include "diodeselector.h"

DiodeSelector::DiodeSelector(QString &title, QDialog *parent):
    QDialog(parent)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 300, 220));
    setWindowTitle(title);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void DiodeSelector::accept()
{
    if (singleDiodeRadioButton->isChecked()) done(1);
    if (commonAnodeRadioButton->isChecked()) done(2);
    if (commonCathodeRadioButton->isChecked()) done(3);
    if (connectedInSeriesRadioButton->isChecked()) done(4);

    done(0);
}
