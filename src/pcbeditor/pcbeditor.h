// pcbeditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef PCBEDITOR_H
#define PCBEDITOR_H

#include "board.h"
#include "ui_pcbeditor.h"
#include <QJsonObject>
#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>
#include <QToolButton>

const static char *boardDirectory = "";

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

class PcbEditor : public QMainWindow, private Ui::PcbEditor
{
    Q_OBJECT
    
public:
    explicit PcbEditor(QWidget *parent = nullptr);
    void limit(int &value, int min, int max);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void paintEvent(QPaintEvent *);
    void selectDevice(int &deviceNameID);
    void writeLibraryFile(QString filename, QJsonObject object);
    // Set buttons: left, right, up, down, zoom in, zoom out
    // void buttonsSetEnabled(const char *params);

private slots:
    void about();
    void closeFile();
    void newFile();
    void openFile();
    void saveErrorCheck();
    void saveFile();
    void saveSVG();
    void saveJSON();
    void selectCheckBox();
    void selectPushButton(int number);
    void selectRadioButton();
    void selectToolButton(int number);

private:
    const static int grids = 11;
    const int grid[grids] = {   // um in grid step
        50, 100, 125, 200, 250, 500, 1000, 1250, 2500, 5000, 10000
    };
    const int gridStep = 10;    // pixels
    const int gridX = 100;
    const int gridY = 30;
    const int spaceStep = 100;  // um
    const int widthStep = 100;
    const int maxX = 10000;
    const int maxY = 10000;
    const int minSpace = 100;
    const int maxSpace = 100000;
    const int minWidth = 100;
    const int maxWidth = 100000;
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
    Board board;
    QPoint mousePoint;
    QSignalMapper *checkBoxMapper;
    QSignalMapper *radioButtonMapper;
    QSignalMapper *pushButtonMapper;
    QSignalMapper *toolButtonMapper;
    QCheckBox *checkBox[checkBoxes];
    QRadioButton *radioButton[radioButtons];
    QPushButton *pushButton[pushButtons];
    QToolButton *toolButton[toolButtons];
    std::vector<Line> lines;
};

#endif  // PCBEDITOR_H
