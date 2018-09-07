// main.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "pcbeditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PcbEditor w;
    w.show();
    
    return a.exec();
}
