// localoptions.cpp
// Copyright (C) 2020 Alexander Karpeko

#include "pcbtypes.h"
#include "localoptions.h"
#include <cmath>

LocalOptions::LocalOptions(LocalOptionsData &options, QWidget *parent):
    QDialog(parent), options(&options)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 300, 200));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    init();
}

void LocalOptions::accept()
{
    bool ok[2];

    int viaDiameter = viaDiameterLineEdit->text().toInt(&ok[0]);
    int viaInnerDiameter = viaInnerDiameterLineEdit->text().toInt(&ok[1]);

    if (ok[0] && ok[1])
        if (viaInnerDiameter > 0 && viaDiameter > viaInnerDiameter) {
            options->viaDiameter = viaDiameter;
            options->viaInnerDiameter = viaInnerDiameter;
            done(QDialog::Accepted);
        }

    init();
}

void LocalOptions::init()
{
    viaDiameterLineEdit->setText(QString::number(options->viaDiameter));
    viaInnerDiameterLineEdit->setText(QString::number(options->viaInnerDiameter));
}
