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

constexpr static int packageComboBoxes = 14;
constexpr static int packageLineEdits = 63;

/*
constexpr static int checkBoxes = 13;
constexpr static int pushButtons = 6;
constexpr static int radioButtons = 4;
constexpr static int toolButtons = 44;


enum PushButton
{
    DEC_GRID, DEC_SPACE, DEC_WIDTH,
    INC_GRID, INC_SPACE, INC_WIDTH
};

enum ToolButton
{
    CREATE_GROUPS, DECREASE_STEP, DELETE, DELETE_JUNCTION,
    DELETE_POLYGON, DELETE_NET_SEGMENTS, DELETE_SEGMENT, ENUMERATE,
    FILL_POLYGON, INCREASE_STEP, METER, MOVE,
    MOVE_GROUP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT,
    MOVE_UP, PLACE_ELEMENTS, PLACE_INDUCTOR, PLACE_INDUCTOR2,
    PLACE_JUNCTION, PLACE_LINE, PLACE_NO_CONNECTION, PLACE_NPN_TRANSISTOR,
    PLACE_PNP_TRANSISTOR, PLACE_POLYGON, PLACE_POWER, PLACE_QUARTZ,
    PLACE_SEGMENT, PLACE_SHOTTKY, PLACE_SWITCH, PLACE_ZENER,
    ROUTE_TRACKS, SEGMENT_NETS, SELECT, SET_VALUE,
    SHOW_GROUND_NETS, TABLE_ROUTE, TURN_TO_LEFT, TURN_TO_RIGHT,
    UPDATE_NETS, WAVE_ROUTE, ZOOM_IN, ZOOM_OUT
};
*/
class PackageEditor : public QMainWindow, private Ui::PackageEditor
{
    Q_OBJECT

public:
    explicit PackageEditor(QWidget *parent = nullptr);
    void limit(int &value, int min, int max);

protected:
    //void keyPressEvent(QKeyEvent *event);
    //void mousePressEvent(QMouseEvent *event);

private:
    void paintEvent(QPaintEvent *);
    void showPackageData();
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
    //void selectCheckBox();
    //void selectPushButton(int number);
    //void selectRadioButton();
    //void selectToolButton(int number);
    void updatePackage();

private:
    constexpr static int grids = 11;
    constexpr static int grid[grids] = {   // um in grid step
        50, 100, 125, 200, 250, 500, 1000, 1250, 2500, 5000, 10000
    };
    constexpr static int gridStep = 10;    // pixels
    constexpr static int gridX = 490;
    constexpr static int gridY = 30;
    constexpr static int spaceStep = 100;  // um
    constexpr static int widthStep = 100;
    constexpr static int maxX = 10000;
    constexpr static int maxY = 10000;
    constexpr static int minSpace = 100;
    constexpr static int maxSpace = 100000;
    constexpr static int minWidth = 100;
    constexpr static int maxWidth = 100000;
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
    Package package;
    QPoint mousePoint;
    QSignalMapper *checkBoxMapper;
    QSignalMapper *radioButtonMapper;
    QSignalMapper *pushButtonMapper;
    QSignalMapper *toolButtonMapper;
    QComboBox *packageComboBox[packageComboBoxes];
    QLineEdit *packageLineEdit[packageLineEdits];
    //QCheckBox *checkBox[checkBoxes];
    //QRadioButton *radioButton[radioButtons];
    //QPushButton *pushButton[pushButtons];
    //QToolButton *toolButton[toolButtons];
    //std::vector<Line> lines;
    std::vector<Package> packages;
};

#endif  // PACKAGEEDITOR_H
