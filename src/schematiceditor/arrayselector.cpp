// arrayselector.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "arrayselector.h"
#include "function.h"

ArraySelector::ArraySelector(const QString &title, QDialog *parent):
    QDialog(parent)
{
    setupUi(this);    
    setGeometry(QRect(97, 111, 250, 200));
    setWindowTitle(title);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
}

void ArraySelector::accept()
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

