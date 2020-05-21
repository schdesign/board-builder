// globaloptions.h
// Copyright (C) 2020 Alexander Karpeko

#ifndef GLOBAL_OPTIONS_H
#define GLOBAL_OPTIONS_H

#include "ui_globaloptions.h"
#include <QDialog>

class GlobalOptionsData
{
public:
    double padCornerRadius;
};

class GlobalOptions : public QDialog, private Ui::GlobalOptions
{
    Q_OBJECT

    constexpr static int radioButtons = 2;

    enum RadioButton
    {
        PAD_ABSOLUTE_RADIUS, PAD_RELATIVE_RADIUS
    };

public:
    explicit GlobalOptions(GlobalOptionsData &options, QWidget *parent = nullptr);

private:
    void init();

private slots:
    void accept();
    void selectRadioButton(int number);

private:
    double padCornerRadius;
    int padCornerRadiusType;
    GlobalOptionsData *options;
    QRadioButton *radioButton[radioButtons];
};

#endif  // GLOBAL_OPTIONS_H
