// pcbeditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef PCBEDITOR_H
#define PCBEDITOR_H

#include "board.h"
#include "packageeditor.h"
#include "ui_pcbeditor.h"
#include <QJsonObject>
#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QSignalMapper>
#include <QToolButton>

const static char *boardDirectory = "";

class PcbEditor : public QMainWindow, private Ui::PcbEditor
{
    Q_OBJECT

    constexpr static int checkBoxes = 3;
    constexpr static int pushButtons = 10;
    constexpr static int radioButtons = 4;
    constexpr static int toolButtons = 39;

    enum CheckBox
    {
        FILL_PADS, SHOW_GRID, SHOW_NETS
    };

    enum PushButton
    {
        CENTER, DEC_FONT_SIZE, DEC_GRID, DEC_SPACE, DEC_WIDTH,
        INC_FONT_SIZE, INC_GRID, INC_SPACE, INC_WIDTH, TURNING_RADIUS
    };

    enum ToolButton
    {
        CONNECT_JUMPER, CREATE_GROUPS, DECREASE_STEP, DELETE, DELETE_JUMPER,
        DELETE_JUNCTION, DELETE_NET_SEGMENTS, DELETE_POLYGON, DELETE_SEGMENT,
        DISCONNECT_JUMPER, FILL_POLYGON, INCREASE_STEP, METER, MOVE, MOVE_DOWN,
        MOVE_GROUP, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, NO_ROUND_TURN, PLACE_ELEMENTS,
        PLACE_JUMPER, PLACE_JUNCTION, PLACE_LINE, PLACE_POLYGON, PLACE_SEGMENT,
        ROUND_TURN, ROUTE_TRACKS, SEGMENT_NETS, SELECT, SET_VALUE, SHOW_GROUND_NETS,
        TABLE_ROUTE, TURN_TO_LEFT, TURN_TO_RIGHT, UPDATE_NETS, WAVE_ROUTE, ZOOM_IN,
        ZOOM_OUT
    };

public:
    explicit PcbEditor(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    void centerBoardBorder();
    void paintEvent(QPaintEvent *);
    bool selectJumper(QString &packageName);
    void writeLibraryFile(QString filename, QJsonObject object);
    // Set buttons: left, right, up, down, zoom in, zoom out
    // void buttonsSetEnabled(const char *params);

private slots:
    void about();
    void closeFile();
    void elementOptions();
    void newFile();
    void openFile();
    void openPackageEditor();
    void saveErrorCheck();
    void saveFile();
    void saveSVG();
    void saveJSON();
    void selectCheckBox(int number);
    void selectLayerCheckBox();
    void selectPushButton(int number);
    void selectRadioButton();
    void selectToolButton(int number);

private:
    constexpr static int defaultFontSize = 10;
    constexpr static int defaultGridNumber = 6;
    constexpr static int defaultTurningRadius = 2000;
    constexpr static int grids = 12;
    constexpr static int gridStep = 10;    // pixels
    constexpr static int gridX = 100;
    constexpr static int gridY = 30;
    constexpr static int gridCenterX = 570;
    constexpr static int gridCenterY = 400;
    constexpr static int spaceStep = 100;  // um
    constexpr static int widthStep = 100;
    constexpr static int maxX = 10000;
    constexpr static int maxY = 10000;
    constexpr static int minFontSize = 1;
    constexpr static int maxFontSize = 100;
    constexpr static int minSpace = 100;
    constexpr static int maxSpace = 100000;
    constexpr static int minTurningRadius = 1;
    constexpr static int maxTurningRadius = 1000000;
    constexpr static int minWidth = 100;
    constexpr static int maxWidth = 100000;
    const int grid[grids] =  // um in grid step
    {
        50, 100, 125, 200, 250, 500, 1000,
        1250, 2000, 2500, 5000, 10000
    };
    bool showGrid;
    double scale;
    int centerX;
    int centerY;
    int command;
    int dx;
    int dy;
    int fontSize;
    int gridNumber;
    int orientation;
    int previousCommand;
    int space;
    int step;
    int turningRadius;
    int width;
    Board board;
    PackageEditor packageEditor;
    QPoint mousePoint;
    QSignalMapper *checkBoxMapper;
    QSignalMapper *layerCheckBoxMapper;
    QSignalMapper *radioButtonMapper;
    QSignalMapper *pushButtonMapper;
    QSignalMapper *toolButtonMapper;
    QCheckBox *checkBox[checkBoxes];
    QCheckBox *layerCheckBox[layersNumber];
    QRadioButton *radioButton[radioButtons];
    QPushButton *pushButton[pushButtons];
    QToolButton *toolButton[toolButtons];
    std::vector<Line> lines;
};

#endif  // PCBEDITOR_H
