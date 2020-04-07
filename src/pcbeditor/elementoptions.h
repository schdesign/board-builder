// elementoptions.h
// Copyright (C) 2020 Alexander Karpeko

#ifndef ELEMENT_OPTIONS_H
#define ELEMENT_OPTIONS_H

#include "ui_elementoptions.h"
#include <QDialog>

class ElementOptionsData
{
public:
    double padCornerRadius;
};

class ElementOptions : public QDialog, private Ui::ElementOptions
{
    Q_OBJECT

    constexpr static int radioButtons = 2;

    enum RadioButton
    {
        ABSOLUTE_RADIUS, RELATIVE_RADIUS
    };

public:
    explicit ElementOptions(ElementOptionsData &options, QWidget *parent = nullptr);

private slots:
    void accept();
    void selectRadioButton(int number);

private:
    void init();

    double padCornerRadius;
    int padCornerRadiusType;
    ElementOptionsData *options;
    QRadioButton *radioButton[radioButtons];
};

#endif  // ELEMENT_OPTIONS_H
