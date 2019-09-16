// deviceselector.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef DEVICE_SELECTOR_H
#define DEVICE_SELECTOR_H

#include "ui_deviceselector.h"
#include <QDialog>
#include <QStringList>

class DeviceSelector : public QDialog, private Ui::DeviceSelector
{
    Q_OBJECT

public:
    explicit DeviceSelector(QStringList &deviceNames, QDialog *parent = nullptr);

private slots:
    void accept();

private:
    QStringList &deviceNames;
};

#endif  // DEVICE_SELECTOR_H
