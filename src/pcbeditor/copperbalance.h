// copperbalance.h
// Copyright (C) 2026 Alexander Karpeko

#ifndef COPPER_BALANCE_H
#define COPPER_BALANCE_H

#include "board.h"
#include "types.h"
#include "ui_copperbalance.h"
#include <QDialog>

class CopperBalance : public QDialog, private Ui::CopperBalance
{
    Q_OBJECT

public:
    explicit CopperBalance(const Board &board, QWidget *parent = nullptr);

private:
    void init();
    bool isRectangleBoard(Point &pMin, Point &pMax);

private slots:
    void accept();
    void run();
    void update();

private:
    static constexpr int columns = 4;
    static constexpr int rows = 4;
    static constexpr int defaultMaxMiBImageSize = 256;  // MiB
    static constexpr int defaultStep = 10;              // micrometers
    double bottomAverageCopperArea;
    double topAverageCopperArea;
    double bottomCopperArea[rows][columns];
    double topCopperArea[rows][columns];
    int maxMiBImageSize;
    int step;
    const Board &board;
};

#endif  // COPPER_BALANCE_H
