// package.h
// Copyright (C) 2019 Alexander Karpeko

#ifndef PACKAGE_H
#define PACKAGE_H

#include "types.h"
#include <QString>
#include <vector>

// Rounded rectangle
class Pad
{
public:
    QJsonObject toJson();

    int number;         // number in package
    int net;            // net number
    int width;          // width > height
    int height;
    int radius;
    int orientation;    // up, right
    int x;              // center
    int y;
};

class Package
{
public:
    void clear();
    QJsonObject toJson();

    Border border;
    Ellipse ellipse;
    int nameTextHeight;
    int nameTextX[4];   // up, right, down, left
    int nameTextY[4];
    int referenceTextHeight;
    int referenceTextX[4];
    int referenceTextY[4];
    int refX;
    int refY;
    QString name;
    QString nameTextAlignH;
    QString nameTextAlignV;
    QString referenceTextAlignH;
    QString referenceTextAlignV;
    QString type;
    std::vector<Line> lines;
    std::vector<Pad> pads;
};

/*
class Package
{
public:
    void clear();
    QJsonObject toJson();

    Border border;
    int nameTextHeight;
    int nameTextX[4];   // up, right, down, left
    int nameTextY[4];
    int referenceTextHeight;
    int referenceTextX[4];
    int referenceTextY[4];
    int refX;
    int refY;
    QString name;
    QString nameTextAlignH;
    QString nameTextAlignV;
    QString referenceTextAlignH;
    QString referenceTextAlignV;
    QString type;
    std::vector<Ellipse> ellipses;
    std::vector<Line> lines;
    std::vector<Pad> pads;
};
*/

#endif  // PACKAGE_H

/*
        addPadOrientationComboBox, addPadTypeComboBox, addPadsOrientationComboBox,
        addPadsTypeComboBox, nameTextAlignHComboBox, nameTextAlignVComboBox,
        padType0ShapeComboBox, padType1ShapeComboBox, padType2ShapeComboBox,
        referenceTextAlignHComboBox, referenceTextAlignVComboBox,
        selectedPadOrientationComboBox, selectedPadTypeComboBox, typeComboBox

        addEllipseHLineEdit, addEllipseWLineEdit, addEllipseXLineEdit,
        addEllipseYLineEdit, addLineX1LineEdit, addLineX2LineEdit,
        addLineY1LineEdit, addLineY2LineEdit, addPadNumberLineEdit,
        addPadXLineEdit, addPadYLineEdit, addPadsDxLineEdit,
        addPadsDyLineEdit, addPadsFirstLineEdit, addPadsFirstXLineEdit,
        addPadsFirstYLineEdit, addPadsLastLineEdit, borderBottomLineEdit,
        borderLeftXLineEdit, borderRightXLineEdit, borderTopYLineEdit,
        ellipsesLineEdit, linesLineEdit, nameLineEdit,
        nameTextHeightLineEdit, nameTextDownXLineEdit, nameTextLeftXLineEdit,
        nameTextRightXLineEdit, nameTextUpXLineEdit, nameTextDownYLineEdit,
        nameTextLeftYLineEdit, nameTextRightYLineEdit,nameTextUpYLineEdit,
        padType0LineEdit1, padType0LineEdit2, padType0LineEdit3,
        padType1LineEdit1, padType1LineEdit2, padType1LineEdit3,
        padType2LineEdit1, padType2LineEdit2, padType2LineEdit3,
        padsLineEdit, referenceTextHeightLineEdit, referenceTextDownXLineEdit,
        referenceTextLeftXLineEdit, referenceTextRightXLineEdit, referenceTextUpXLineEdit,
        referenceTextDownYLineEdit, referenceTextLeftYLineEdit, referenceTextRightYLineEdit,
        referenceTextUpYLineEdit, selectedEllipseHLineEdit, selectedEllipseWLineEdit,
        selectedEllipseXLineEdit, selectedEllipseYLineEdit, selectedLineX1LineEdit,
        selectedLineX2LineEdit, selectedLineY1LineEdit, selectedLineY2LineEdit,
        selectedPadNumberLineEdit, selectedPadXLineEdit, selectedPadYLineEdit
*/
















