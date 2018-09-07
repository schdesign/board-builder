// main.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "schematiceditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SchematicEditor w;
    w.show();
    
    return a.exec();
}
