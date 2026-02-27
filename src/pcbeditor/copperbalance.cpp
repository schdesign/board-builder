// copperbalance.cpp
// Copyright (C) 2026 Alexander Karpeko

#include "copperbalance.h"

CopperBalance::CopperBalance(QWidget *parent):
    QDialog(parent)
{
    setupUi(this);
    setGeometry(QRect(101, 108, 300, 400));

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void CopperBalance::accept()
{
    done(QDialog::Accepted);
}
