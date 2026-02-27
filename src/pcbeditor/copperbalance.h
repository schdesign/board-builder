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

private slots:
    void accept();
};

#endif  // COPPER_BALANCE_H
