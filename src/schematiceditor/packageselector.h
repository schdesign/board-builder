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
    explicit PackageSelector(Schematic &schematic, QDialog *parent = nullptr);

private slots:
    void accept();
    void clearPackage();
    void connectPackage();

private:
    Schematic &schematic;
    std::vector<int> elementPins;
    std::vector<QString> elementNames;
    std::vector<QString> packageNames;
};

#endif  // PACKAGESELECTOR_H
