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

bool LessReference::operator() (const QString &str, const QString &str2)
{
    bool digits = false;
    int i;
    int size = str.size();
    int size2 = str2.size();

    // Compare letters
    for (i = 0; i < size; i++) {
        if (i >= size2)
            return false;
        if (str[i].isDigit() && str2[i].isDigit()) {
            digits = true;
            break;
        }
        if (str[i] < str2[i])
            return true;
        if (str[i] > str2[i])
            return false;
    }

    if (!digits) {
        if (size < size2)
            return true;
        return false;  // equal strings
    }

    // Compare numbers without additional check
    int n = str.rightRef(size - i).toInt();
    int n2 = str2.rightRef(size2 - i).toInt();
    return n < n2;
}

void limit(int &value, int min, int max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
}
