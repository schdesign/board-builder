// deviceselector.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "deviceselector.h"
#include <QRegularExpression>

DeviceSelector::DeviceSelector(QStringList &deviceNames, QDialog *parent):
    QDialog(parent), deviceNames(deviceNames)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 250, 300));

    deviceListWidget->insertItems(0, deviceNames);
    deviceListWidget->sortItems();
    deviceListWidget->setCurrentRow(0);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void DeviceSelector::accept()
{
    QString str = deviceListWidget->currentItem()->text();
    QRegularExpression re(str);
    int i = deviceNames.indexOf(re);
    done(i + 1);
}
