// deviceselector.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "deviceselector.h"

DeviceSelector::DeviceSelector(QStringList &deviceNames, QDialog *parent):
    QDialog(parent), deviceNames(deviceNames)
{
    setupUi(this);
    setGeometry(QRect(93, 100, 250, 300));

    deviceListWidget->insertItems(0, deviceNames);
    deviceListWidget->setCurrentRow(0);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void DeviceSelector::accept()
{
    int i = deviceListWidget->currentRow();
    done(i + 1);
}
