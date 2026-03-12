// copperbalance.h
// Copyright (C) 2026 Alexander Karpeko

#ifndef COPPER_BALANCE_H
#define COPPER_BALANCE_H

#include "ui_copperbalance.h"
#include <QDialog>

class CopperBalance : public QDialog, private Ui::CopperBalance
{
    Q_OBJECT

public:
    explicit CopperBalance(QWidget *parent = nullptr);

private:
    void init();

private slots:
    void accept();
    void run();
    void update();

private:
    static constexpr int columns = 4;
    static constexpr int rows = 4;
    static constexpr int maxImageSize = 256;  // MiB
    static constexpr int step = 10;           // micrometers
    int bottomAverageCopperArea;
    int topAverageCopperArea;
    int bottomCopperArea[rows][columns];
    int topCopperArea[rows][columns];
    QImage *qimage;
};

#endif  // COPPER_BALANCE_H
