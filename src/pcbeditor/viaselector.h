// viaselector.h
// Copyright (C) 2015-2017 Alexander Karpeko

#ifndef VIA_SELECTOR_H
#define VIA_SELECTOR_H

#include "ui_viaselector.h"
#include <QDialog>

const int maxArrayNumber = 100; // max number of elements

class ViaSelector : public QDialog, private Ui::ViaSelector
{
    Q_OBJECT

public:
    explicit ViaSelector(const QString &title, QDialog *parent = 0);

private slots:
    void accept();
};

#endif  // VIA_SELECTOR_H

