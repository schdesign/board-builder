// viaoptions.h
// Copyright (C) 2020 Alexander Karpeko

#ifndef VIA_OPTIONS_H
#define VIA_OPTIONS_H

#include "ui_viaoptions.h"
#include <QDialog>

class ViaOptionsData
{
public:
    int diameter;
    int innerDiameter;
};

class ViaOptions : public QDialog, private Ui::ViaOptions
{
    Q_OBJECT

public:
    explicit ViaOptions(ViaOptionsData &options, QWidget *parent = nullptr);

private slots:
    void accept();

private:
    void init();

    ViaOptionsData *options;
};

#endif  // VIA_OPTIONS_H
