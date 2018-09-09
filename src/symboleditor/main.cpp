// main.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "symboleditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SymbolEditor w;
    w.show();

    return a.exec();
}
