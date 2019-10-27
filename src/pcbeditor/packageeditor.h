// packageeditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef PACKAGEEDITOR_H
#define PACKAGEEDITOR_H

#include "ui_packageeditor.h"
#include <QMainWindow>

class PackageEditor : public QMainWindow, private Ui::PackageEditor
{
    Q_OBJECT

public:
    explicit PackageEditor(QWidget *parent = nullptr);
};

#endif  // PACKAGEEDITOR_H
