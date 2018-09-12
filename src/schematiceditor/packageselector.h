// packageselector.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef PACKAGESELECTOR_H
#define PACKAGESELECTOR_H

#include "schematic.h"
#include "ui_packageselector.h"
#include <QDialog>
#include <QStringList>

class PackageSelector : public QDialog, private Ui::PackageSelector
{
    Q_OBJECT

public:
    explicit PackageSelector(Schematic &schematic, QDialog *parent = 0);

private:
    Schematic &schematic;
    std::vector<int> elementPins;
    std::vector<QString> elementNames;
    std::vector<QString> packageNames;

private slots:
    void accept();
    void clearPackage();
    void connectPackage();
};

#endif  // PACKAGESELECTOR_H
