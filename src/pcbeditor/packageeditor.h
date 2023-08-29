// packageeditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef PACKAGEEDITOR_H
#define PACKAGEEDITOR_H

#include "element.h"
#include "layers.h"
#include "package.h"
#include "ui_packageeditor.h"
#include <QJsonObject>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>
#include <QToolButton>

static const char *packageDirectory = "";

class PackageEditor : public QMainWindow, private Ui::PackageEditor
{
    Q_OBJECT

    static constexpr int checkBoxes = 3;
    static constexpr int comboBoxes = 10;
    static constexpr int lineEdits = 55;
    static constexpr int pushButtons = 5;
    static constexpr int radioButtons = 15;

    enum CheckBox
    {
        SHOW_BORDER, SHOW_PACKAGE, SHOW_PADS
    };

    enum ComboBox
    {
        ADD_PAD_ORIENTATION, ADD_PAD_TYPE, ADD_PADS_ORIENTATION,
        ADD_PADS_TYPE, PAD_TYPE_0_SHAPE, PAD_TYPE_1_SHAPE,
        PAD_TYPE_2_SHAPE, SELECTED_PAD_ORIENTATION,
        SELECTED_PAD_TYPE, PACKAGE_TYPE
    };

    enum PushButton
    {
        CANCEL, CENTER, DEC_GRID, INC_GRID, UPDATE
    };

    enum RadioButton
    {
        ADD_ELLIPSE, ADD_LINE, ADD_PAD, ADD_PADS, BORDER, DELETE_ELLIPSES,
        DELETE_LINES, DELETE_PADS, NAME, PAD_TYPES, READ_ONLY_MODE,
        SELECTED_ELLIPSE, SELECTED_LINE, SELECTED_PAD, TYPE
    };

public:
    explicit PackageEditor(QWidget *parent = nullptr);

protected:
    //void keyPressEvent(QKeyEvent *event);
    //void mousePressEvent(QMouseEvent *event);

private:
    void cancelPackageChange();
    void centerElement();
    QString padShape(const PadTypeParams &padTypeParams);
    void paintEvent(QPaintEvent *);
    void readPackages(const QByteArray &byteArray);
    void selectRadioButton(int number, bool state);
    void setPadTypeLineEdit(QLabel *label, const QString &labelText, QLineEdit *lineEdit,
                            const QString &lineEditText, bool state);
    void setPadTypeTexts(int padTypeNumber, int n1, int n2, int n3, int n4);
    void setRadioButton(QRadioButton *button, bool state);
    void showLayer(int number, bool state);
    void showPackageData();
    void updatePackage();
    void updateElement();
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
    void selectComboBox(int number, int index);
    void selectPadTypeComboBox(int number, const QString &text);
    void selectPushButton(int number);
    void selectRadioButton(int number);
    //void selectToolButton(int number);

private:
    static constexpr int grids = 12;
    static constexpr int gridStep = 10;    // pixels
    static constexpr int gridWidth = 680;
    static constexpr int gridHeight = 600;
    static constexpr int gridX = 570;
    static constexpr int gridY = 30;
    static constexpr int spaceStep = 100;  // um
    static constexpr int widthStep = 100;
    static constexpr int maxX = 10000;
    static constexpr int maxY = 10000;
    static constexpr int minSpace = 100;
    static constexpr int maxSpace = 100000;
    static constexpr int minWidth = 100;
    static constexpr int maxWidth = 100000;
    static constexpr int maxPads = 10000;
    static constexpr int maxPadTypes = 3;
    static constexpr int maxPadParams = 4;
    const int grid[grids] =  // um in grid step
    {
        10, 20, 25, 50, 100, 125, 200,
        250, 500, 1000, 1250, 2000
    };
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
    int refX;
    int refY;
    int space;
    int step;
    int width;
    ulong selectedEllipseIndex;
    ulong selectedLineIndex;
    ulong selectedPadIndex;
    Element element;
    Layers layers;
    Package package;
    Package tmpPackage;
    QPoint mousePoint;
    QSignalMapper *checkBoxMapper;
    QSignalMapper *pushButtonMapper;
    QSignalMapper *radioButtonMapper;
    QSignalMapper *toolButtonMapper;
    QString padTypeShape[maxPadTypes];
    QCheckBox *checkBox[checkBoxes];
    QComboBox *comboBox[comboBoxes];
    QComboBox *padTypeShapeComboBox[maxPadTypes];
    QLineEdit *lineEdit[lineEdits];
    QLineEdit *padTypeLineEdit[maxPadTypes][maxPadParams];
    QPushButton *pushButton[pushButtons];
    QRadioButton *radioButton[radioButtons];
    //QToolButton *toolButton[toolButtons];
    std::vector<Package> packages;
};

#endif  // PACKAGEEDITOR_H
