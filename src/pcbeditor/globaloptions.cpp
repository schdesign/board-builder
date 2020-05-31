// globaloptions.cpp
// Copyright (C) 2020 Alexander Karpeko

#include "element.h"
#include "globaloptions.h"
#include "function.h"
#include "types.h"
#include <cmath>

GlobalOptions::GlobalOptions(GlobalOptionsData &options, QWidget *parent):
    QDialog(parent), options(&options)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 550, 200));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QRadioButton *tmpRadioButton[radioButtons] =
    {
        padAbsoluteRadiusRadioButton, padRelativeRadiusRadioButton
    };

    std::copy(tmpRadioButton, tmpRadioButton + radioButtons, radioButton);

    for (int i = 0; i < radioButtons; i++)
        connect(radioButton[i], &QRadioButton::clicked, [=] () { selectRadioButton(i); });

    init();
}

void GlobalOptions::accept()
{
    constexpr int size = 3;
    bool ok[size];
    int n = 0;

    ok[n++] = getOpenMaskOnVia();
    ok[n++] = getPadCornerRadius();
    ok[n++] = getSolderMaskSwell();

    for (int i = 0; i < size; i++)
        if (!ok[i])
            return;

    done(QDialog::Accepted);
}

bool GlobalOptions::getOpenMaskOnVia()
{
    options->openMaskOnVia = openMaskOnViaCheckBox->isChecked();
    return true;
}

bool GlobalOptions::getPadCornerRadius()
{
    bool ok;

    if (padCornerRadiusType == PAD_ABSOLUTE_RADIUS) {
        int n = padAbsoluteRadiusLineEdit->text().toInt(&ok);
        if (ok) {
            if (n >= 0 && n <= lround(Element::maxPadCornerRadius)) {
                options->padCornerRadius = n;
                return true;
            }
            else {
                limit(n, 0, lround(Element::maxPadCornerRadius));
                padCornerRadius = n;
            }
        }
        padAbsoluteRadiusLineEdit->setText(QString::number(lround(padCornerRadius)));
    }
    else {
        double d = padRelativeRadiusLineEdit->text().toDouble(&ok);
        if (ok) {
            if (d > Element::minPadCornerRadius - minValue && d < 0.5 + minValue) {
                options->padCornerRadius = d;
                return true;
            }
            else {
                if (d < Element::minPadCornerRadius)
                    d = Element::minPadCornerRadius;
                if (d > 0.5)
                    d = 0.5;
                padCornerRadius = d;
            }
        }
        padRelativeRadiusLineEdit->setText(QString::number(padCornerRadius));
    }

    return false;
}

bool GlobalOptions::getSolderMaskSwell()
{
    bool ok;

    int n = solderMaskSwellLineEdit->text().toInt(&ok);
    if (ok) {
        if (n >= 0) {
            options->solderMaskSwell = n;
            return true;
        }
        else
            solderMaskSwell = 0;
    }
    solderMaskSwellLineEdit->setText(QString::number(solderMaskSwell));

    return false;
}

void GlobalOptions::init()
{
    setOpenMaskOnVia();
    setPadCornerRadius();
    setSolderMaskSwell();
}

void GlobalOptions::setOpenMaskOnVia()
{
    openMaskOnViaCheckBox->setChecked(options->openMaskOnVia);
}

void GlobalOptions::setPadCornerRadius()
{
    double r = options->padCornerRadius;

    padCornerRadius = 0;
    padCornerRadiusType = PAD_ABSOLUTE_RADIUS;

    if (r > 1 - minValue) {
        if (r > Element::maxPadCornerRadius)
            r = Element::maxPadCornerRadius;
        padCornerRadius = r;
    }

    if (r > Element::minPadCornerRadius - minValue && r < 0.5 + minValue) {
        padCornerRadius = r;
        padCornerRadiusType = PAD_RELATIVE_RADIUS;
    }

    if (padCornerRadiusType == PAD_ABSOLUTE_RADIUS) {
        padAbsoluteRadiusLineEdit->setText(QString::number(lround(padCornerRadius)));
        padAbsoluteRadiusRadioButton->click();
    }
    else {
        padRelativeRadiusLineEdit->setText(QString::number(padCornerRadius));
        padRelativeRadiusRadioButton->click();
    }
}

void GlobalOptions::selectRadioButton(int number)
{
    switch (number) {
    case PAD_ABSOLUTE_RADIUS:
        padAbsoluteRadiusLineEdit->setReadOnly(false);
        padRelativeRadiusLineEdit->setReadOnly(true);
        padCornerRadiusType = number;
        break;
    case PAD_RELATIVE_RADIUS:
        padAbsoluteRadiusLineEdit->setReadOnly(true);
        padRelativeRadiusLineEdit->setReadOnly(false);
        padCornerRadiusType = number;
        break;
    }
}

void GlobalOptions::setSolderMaskSwell()
{
    solderMaskSwell = 0;
    if (options->solderMaskSwell > 0)
        solderMaskSwell = options->solderMaskSwell;
    solderMaskSwellLineEdit->setText(QString::number(solderMaskSwell));
}
