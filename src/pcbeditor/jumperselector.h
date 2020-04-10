// jumperselector.h
// Copyright (C) 2020 Alexander Karpeko

#ifndef JUMPER_SELECTOR_H
#define JUMPER_SELECTOR_H

#include "ui_jumperselector.h"
#include <QDialog>
#include <QStringList>

class JumperSelector : public QDialog, private Ui::JumperSelector
{
    Q_OBJECT

public:
    explicit JumperSelector(const QStringList &jumperNames, QString &packageName,
                            QWidget *parent = nullptr);

private slots:
    void accept();

private:
    const QStringList &jumperNames;
    QString &packageName;
};

#endif  // JUMPER_SELECTOR_H
