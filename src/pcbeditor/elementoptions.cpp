// elementoptions.cpp
// Copyright (C) 2020 Alexander Karpeko

#include "element.h"
#include "elementoptions.h"
#include "function.h"
#include "types.h"
#include <cmath>

ElementOptions::ElementOptions(ElementOptionsData &options, QWidget *parent):
    QDialog(parent), options(&options)
{
    setupUi(this);
    setGeometry(QRect(97, 111, 300, 200));

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QRadioButton *tmpRadioButton[radioButtons] =
    {
        absoluteRadiusRadioButton, relativeRadiusRadioButton
    };

    std::copy(tmpRadioButton, tmpRadioButton + radioButtons, radioButton);

    for (int i = 0; i < radioButtons; i++)
        connect(radioButton[i], &QRadioButton::clicked, [=] () { selectRadioButton(i); });

    init();
}

void ElementOptions::accept()
{
    bool ok;

    if (padCornerRadiusType == ABSOLUTE_RADIUS) {
        int n = absoluteRadiusLineEdit->text().toInt(&ok);
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
        absoluteRadiusLineEdit->setText(QString::number(lround(padCornerRadius)));
        return;
    }
    else {
        double d = relativeRadiusLineEdit->text().toDouble(&ok);
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
        relativeRadiusLineEdit->setText(QString::number(padCornerRadius));
        return;
    }
}

void ElementOptions::init()
{
    double r = options->padCornerRadius;

    padCornerRadius = 0;
    padCornerRadiusType = ABSOLUTE_RADIUS;

    if (r > 1 - minValue) {
        if (r > Element::maxPadCornerRadius)
            r = Element::maxPadCornerRadius;
        padCornerRadius = r;
    }

    if (r > Element::minPadCornerRadius - minValue && r < 0.5 + minValue) {
        padCornerRadius = r;
        padCornerRadiusType = RELATIVE_RADIUS;
    }

    if (padCornerRadiusType == ABSOLUTE_RADIUS) {
        absoluteRadiusLineEdit->setText(QString::number(lround(padCornerRadius)));
        absoluteRadiusRadioButton->click();
    }
    else {
        relativeRadiusLineEdit->setText(QString::number(padCornerRadius));
        relativeRadiusRadioButton->click();
    }
}

void ElementOptions::selectRadioButton(int number)
{
    switch (number) {
    case ABSOLUTE_RADIUS:
        absoluteRadiusLineEdit->setReadOnly(false);
        relativeRadiusLineEdit->setReadOnly(true);
        padCornerRadiusType = number;
        break;
    case RELATIVE_RADIUS:
        absoluteRadiusLineEdit->setReadOnly(true);
        relativeRadiusLineEdit->setReadOnly(false);
        padCornerRadiusType = number;
        break;
    }
}
