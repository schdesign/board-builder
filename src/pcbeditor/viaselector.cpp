// viaselector.cpp
// Copyright (C) 2015-2017 Alexander Karpeko

#include "viaselector.h"
#include "function.h"

ViaSelector::ViaSelector(const QString &title, QDialog *parent):
    QDialog(parent)
{
    setupUi(this);    
    setGeometry(QRect(93, 100, 250, 200));
    setWindowTitle(title);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ViaSelector::accept()
{
    int array;
    QString str;

    int number = numberLineEdit->text().toInt();
    if (number < 1 || number > maxArrayNumber) {
        limit(number, 1, maxArrayNumber);
        numberLineEdit->setText(str.setNum(number));
        return;
    }

    int orientation = 0;
    if (rightRadioButton->isChecked())
        orientation = 1;

    array = (number << 1) + orientation;
    done(array);
}

