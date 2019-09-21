// diodeselector.h
// Copyright (C) 2019 Alexander Karpeko

#ifndef DIODE_SELECTOR_H
#define DIODE_SELECTOR_H

#include "elementimage.h"
#include "ui_diodeselector.h"
#include <QDialog>

class DiodeSelector : public QDialog, private Ui::DiodeSelector
{
    Q_OBJECT

public:
    explicit DiodeSelector(QString &title, QDialog *parent = nullptr);

private slots:
    void accept();

private:
    int selectorType;
};

#endif  // DIODE_SELECTOR_H
