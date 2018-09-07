// function.h
// Copyright (C) 2018 Alexander Karpeko

#include "function.h"

bool findIndex(int &type, const QString &typeString,
              const char typeStrings[][32], int size)
{
    for (int i = 0; i < size; i++)
        if (!typeString.compare(typeStrings[i])) {
            type = i;
            return true;
        }

    return false;
}

QJsonArray intArrayToJson(const int n[], int size)
{
    QJsonArray array;

    for (int i = 0; i < size; i++)
        array.append(n[i]);

    return array;
}

void jsonToIntArray(const QJsonValue &value, int n[], int size)
{
    QJsonArray array = value.toArray();

    for (int i = 0; i < size; i++)
        n[i] = array[i].toInt();
}

void limit(int &value, int min, int max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
}
