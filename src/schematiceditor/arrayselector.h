// arrayselector.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef ARRAY_SELECTOR_H
#define ARRAY_SELECTOR_H

#include "ui_arrayselector.h"
#include <QDialog>

constexpr int maxArrayNumber = 100; // max number of elements

class ArraySelector : public QDialog, private Ui::ArraySelector
{
    Q_OBJECT

public:
    explicit ArraySelector(const QString &title, QDialog *parent = nullptr);

private slots:
    void accept();
};

#endif  // ARRAY_SELECTOR_H

