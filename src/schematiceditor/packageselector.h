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

private:
    template<typename Type>
    void addData(const Type &type);
    void drawElement(QPainter &painter, int row);
    void drawPackage(QPainter &painter, int row);
    int padsMapFromString(QString &str, int size);
    void paintEvent(QPaintEvent *);
    void scalePackage(const Package &package, double length, double &scale);
    void updateItem(int row);

private slots:
    void accept();
    void clearPackage();
    void connectPackage();
    void itemSelectionChanged();
    void mapPads();

private:
    constexpr static int packageWindowWidth = 120;
    constexpr static int packageWindowHeight = 120;
    constexpr static int packageWindowX = 620;
    constexpr static int packageWindowY = 190;
    constexpr static int symbolWindowWidth = 120;
    constexpr static int symbolWindowHeight = 120;
    constexpr static int symbolWindowX = 620;
    constexpr static int symbolWindowY = 30;
    int firstElementRow;
    int pastLastElementRow;
    Schematic &schematic;
    std::vector<int> centers;
    std::vector<int> padsMaps;
    std::vector<int> pins;
    std::vector<QString> names;
    std::vector<QString> packageNames;
};

#endif  // PACKAGESELECTOR_H
