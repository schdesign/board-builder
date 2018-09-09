// main.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "packageeditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PackageEditor w;
    w.show();

    return a.exec();
}
