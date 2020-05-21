// localoptions.h
// Copyright (C) 2020 Alexander Karpeko

#ifndef LOCAL_OPTIONS_H
#define LOCAL_OPTIONS_H

#include "ui_localoptions.h"
#include <QDialog>

class LocalOptionsData
{
public:
    int viaDiameter;
    int viaInnerDiameter;
};

class LocalOptions : public QDialog, private Ui::LocalOptions
{
    Q_OBJECT

public:
    explicit LocalOptions(LocalOptionsData &options, QWidget *parent = nullptr);

private:
    void init();

private slots:
    void accept();

private:
    LocalOptionsData *options;
};

#endif  // LOCAL_OPTIONS_H
