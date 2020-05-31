// globaloptions.h
// Copyright (C) 2020 Alexander Karpeko

#ifndef GLOBAL_OPTIONS_H
#define GLOBAL_OPTIONS_H

#include "ui_globaloptions.h"
#include <QDialog>

class GlobalOptionsData
{
public:
    bool openMaskOnVia;
    double padCornerRadius;
    int solderMaskSwell;
};

class GlobalOptions : public QDialog, private Ui::GlobalOptions
{
    Q_OBJECT

    static constexpr int radioButtons = 2;

    enum RadioButton
    {
        PAD_ABSOLUTE_RADIUS, PAD_RELATIVE_RADIUS
    };

public:
    explicit GlobalOptions(GlobalOptionsData &options, QWidget *parent = nullptr);

private:
    bool getOpenMaskOnVia();
    bool getPadCornerRadius();
    bool getSolderMaskSwell();
    void init();
    void setOpenMaskOnVia();
    void setPadCornerRadius();
    void setSolderMaskSwell();

private slots:
    void accept();
    void selectRadioButton(int number);

private:
    double padCornerRadius;
    int padCornerRadiusType;
    int solderMaskSwell;
    GlobalOptionsData *options;
    QRadioButton *radioButton[radioButtons];
};

#endif  // GLOBAL_OPTIONS_H
