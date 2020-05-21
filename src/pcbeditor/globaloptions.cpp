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
    setGeometry(QRect(97, 111, 300, 200));

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
    bool ok;

    if (padCornerRadiusType == PAD_ABSOLUTE_RADIUS) {
        int n = padAbsoluteRadiusLineEdit->text().toInt(&ok);
        if (ok) {
            if (n >= 0 && n <= lround(Element::maxPadCornerRadius)) {
                options->padCornerRadius = n;
                done(QDialog::Accepted);
            }
            else {
                limit(n, 0, lround(Element::maxPadCornerRadius));
                padCornerRadius = n;
            }
        }
        padAbsoluteRadiusLineEdit->setText(QString::number(lround(padCornerRadius)));
        return;
    }
    else {
        double d = padRelativeRadiusLineEdit->text().toDouble(&ok);
        if (ok) {
            if (d > Element::minPadCornerRadius - minValue && d < 0.5 + minValue) {
                options->padCornerRadius = d;
                done(QDialog::Accepted);
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
        return;
    }
}

void GlobalOptions::init()
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
