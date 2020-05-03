// viaoptions.cpp
// Copyright (C) 2020 Alexander Karpeko

#include "pcbtypes.h"
#include "viaoptions.h"
#include <cmath>

ViaOptions::ViaOptions(ViaOptionsData &options, QWidget *parent):
    QDialog(parent), options(&options)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 300, 200));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    init();
}

void ViaOptions::accept()
{
    bool ok[2];

    int diameter = diameterLineEdit->text().toInt(&ok[0]);
    int innerDiameter = innerDiameterLineEdit->text().toInt(&ok[1]);

    if (ok[0] && ok[1])
        if (innerDiameter > 0 && diameter > innerDiameter) {
                options->diameter = diameter;
                options->innerDiameter = innerDiameter;
                done(QDialog::Accepted);
            }

    init();
}

void ViaOptions::init()
{
    diameterLineEdit->setText(QString::number(options->diameter));
    innerDiameterLineEdit->setText(QString::number(options->innerDiameter));
}
