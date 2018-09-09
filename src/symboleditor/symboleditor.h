// symboleditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SYMBOLEDITOR_H
#define SYMBOLEDITOR_H

#include "ui_symboleditor.h"
#include <QMainWindow>

class SymbolEditor : public QMainWindow, private Ui::SymbolEditor
{
    Q_OBJECT

public:
    explicit SymbolEditor(QWidget *parent = 0);
};

#endif  // SYMBOLEDITOR_H
