// copperbalance.cpp
// Copyright (C) 2026 Alexander Karpeko

#include "copperbalance.h"

CopperBalance::CopperBalance(QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    setGeometry(QRect(101, 108, 300, 520));

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));

    init();
}

void CopperBalance::accept()
{
    done(QDialog::Accepted);
}

void CopperBalance::init()
{
    stepLineEdit->setText(QString::number(step));
    maxImageSizeLineEdit->setText(QString::number(maxImageSize));
}

void CopperBalance::run()
{
    topAverageCopperArea = 0;
    bottomAverageCopperArea = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++) {
            topAverageCopperArea += topCopperArea[i][j];
            bottomAverageCopperArea += bottomCopperArea[i][j];
    }
    topAverageCopperArea /= (rows * columns);
    bottomAverageCopperArea /= (rows * columns);
}

void CopperBalance::update()
{

}
