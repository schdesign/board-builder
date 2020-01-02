// packageeditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef PACKAGEEDITOR_H
#define PACKAGEEDITOR_H

#include "package.h"
#include "ui_packageeditor.h"
#include <QJsonObject>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>
#include <QToolButton>

const static char *packageDirectory = "";

class PackageEditor : public QMainWindow, private Ui::PackageEditor
{
    Q_OBJECT

    constexpr static int checkBoxes = 5;
    constexpr static int comboBoxes = 14;
    constexpr static int lineEdits = 68;
    constexpr static int pushButtons = 2;
    constexpr static int radioButtons = 14;

    enum CheckBox
    {
        SHOW_BORDER, SHOW_ELLIPSES, SHOW_LINES,
        SHOW_PADS, SHOW_TEXT
    };

    enum ComboBox
    {
        ADD_PAD_ORIENTATION, ADD_PAD_TYPE, ADD_PADS_ORIENTATION,
        ADD_PADS_TYPE, NAME_TEXT_ALIGN_H, NAME_TEXT_ALIGN_V,
        PAD_TYPE_0_SHAPE, PAD_TYPE_1_SHAPE, PAD_TYPE_2_SHAPE,
        REFERENCE_TEXT_ALIGN_H, REFERENCE_TEXT_ALIGN_V,
        SELECTED_PAD_ORIENTATION, SELECTED_PAD_TYPE, PACKAGE_TYPE
    };

    enum PushButton
    {
        CANCEL, UPDATE
    };

    enum RadioButton
    {
        ADD_ELLIPSE, ADD_LINE, ADD_PAD, ADD_PADS, BORDER, NAME,
        PAD_TYPES, READ_ONLY_MODE, SELECTED_ELLIPSE, SELECTED_LINE,
        SELECTED_PAD, TEXT_PARAMS, TEXT_PLACE, TYPE
    };

public:
    explicit PackageEditor(QWidget *parent = nullptr);
    void limit(int &value, int min, int max);

protected:
    //void keyPressEvent(QKeyEvent *event);
    //void mousePressEvent(QMouseEvent *event);

private:
    void cancelPackageChange();
    void paintEvent(QPaintEvent *);
    void selectRadioButton(int number, bool state);
    void setRadioButton(QRadioButton *button, bool state);
    void showPackageData();
    void updatePackage();
    //void selectDevice(int &deviceNameID);
    //void writeLibraryFile(QString filename, QJsonObject object);
    // Set buttons: left, right, up, down, zoom in, zoom out
    // void buttonsSetEnabled(const char *params);

private slots:
    void about();
    void closeFile();
    void newFile();
    void newPackage();
    void openFile();
    void saveFile();
    void selectCheckBox(int number);
    void selectComboBox(int number, const QString &text);
    void selectPadTypeComboBox(int number, const QString &text);
    void selectPushButton(int number);
    void selectRadioButton(int number);
    //void selectToolButton(int number);

private:
    constexpr static int grids = 11;
    constexpr static int grid[grids] = {   // um in grid step
        50, 100, 125, 200, 250, 500, 1000, 1250, 2500, 5000, 10000
    };
    constexpr static int gridStep = 10;    // pixels
    constexpr static int gridX = 570;
    constexpr static int gridY = 30;
    constexpr static int spaceStep = 100;  // um
    constexpr static int widthStep = 100;
    constexpr static int maxX = 10000;
    constexpr static int maxY = 10000;
    constexpr static int minSpace = 100;
    constexpr static int maxSpace = 100000;
    constexpr static int minWidth = 100;
    constexpr static int maxWidth = 100000;
    constexpr static int maxPads = 10000;
    constexpr static int padTypes = 3;
    bool isEllipseSelected;
    bool isLineSelected;
    bool isPadSelected;
    double scale;
    int centerX;
    int centerY;
    int command;
    int dx;
    int dy;
    int gridNumber;
    int orientation;
    int previousCommand;
    int space;
    int step;
    int width;
    ulong selectedEllipseIndex;
    ulong selectedLineIndex;
    ulong selectedPadIndex;
    Package package;
    Package tmpPackage;
    QPoint mousePoint;
    QSignalMapper *checkBoxMapper;
    QSignalMapper *radioButtonMapper;
    QSignalMapper *pushButtonMapper;
    QSignalMapper *toolButtonMapper;
    QString elementName;
    QString elementReference;
    QString padTypeShape[padTypes];
    QCheckBox *checkBox[checkBoxes];
    QComboBox *comboBox[comboBoxes];
    QLineEdit *lineEdit[lineEdits];
    QPushButton *pushButton[pushButtons];
    QRadioButton *radioButton[radioButtons];
    //QToolButton *toolButton[toolButtons];
    //std::vector<Line> lines;
    std::vector<Package> packages;
};

#endif  // PACKAGEEDITOR_H
