// jumperselector.cpp
// Copyright (C) 2020 Alexander Karpeko

#include "jumperselector.h"

JumperSelector::JumperSelector(const QStringList &jumperNames, QString &packageName,
                               QWidget *parent):
    QDialog(parent), jumperNames(jumperNames), packageName(packageName)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 250, 300));

    jumperListWidget->insertItems(0, jumperNames);
    jumperListWidget->setCurrentRow(0);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void JumperSelector::accept()
{
    packageName = jumperListWidget->currentItem()->text();
    done(QDialog::Accepted);
}
