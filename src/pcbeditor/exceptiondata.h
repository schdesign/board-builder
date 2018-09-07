// exceptiondata.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef EXCEPTIONDATA_H
#define EXCEPTIONDATA_H

#include <QString>
#include <stdexcept>

class ExceptionData : public std::exception
{
public:
    ExceptionData(QString str) : message(str) {}
    ~ExceptionData() throw() {}
    QString show() { return message; }

private:
    QString message;
};

#endif  // EXCEPTIONDATA_H
