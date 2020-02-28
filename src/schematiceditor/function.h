// function.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef FUNCTION_H
#define FUNCTION_H

#include <QJsonArray>
#include <QJsonValue>
#include <QString>

bool findIndex(int &type, const QString &typeString,
               const char typeStrings[][32], int size);

QJsonArray intArrayToJson(const int n[], int size);

void jsonToIntArray(const QJsonValue &value, int n[], int size);

struct LessReference
{
    bool operator() (const QString &str, const QString &str2) const;
};

void limit(int &value, int min, int max);

#endif  // FUNCTION_H
