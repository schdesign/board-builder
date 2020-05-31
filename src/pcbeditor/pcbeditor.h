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

static const char *boardDirectory = "";

class PcbEditor : public QMainWindow, private Ui::PcbEditor
{
    Q_OBJECT

    static constexpr int checkBoxes = 3;
    static constexpr int pushButtons = 10;
    static constexpr int radioButtons = 4;
    static constexpr int toolButtons = 39;

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
        DELETE_NET_SEGMENTS, DELETE_POLYGON, DELETE_SEGMENT, DELETE_VIA,
        DISCONNECT_JUMPER, FILL_POLYGON, INCREASE_STEP, METER, MOVE, MOVE_DOWN,
        MOVE_GROUP, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, NO_ROUND_TURN, PLACE_ELEMENTS,
        PLACE_JUMPER, PLACE_LINE, PLACE_POLYGON, PLACE_SEGMENT, PLACE_VIA,
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
    void globalOptions();
    void localOptions();
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
    static constexpr int defaultFontSize = 10;
    static constexpr int defaultGridNumber = 6;
    static constexpr int defaultTurningRadius = 2000;
    static constexpr int defaultViaDiameter = 1000;
    static constexpr int defaultViaInnerDiameter = 500;
    static constexpr int grids = 12;
    static constexpr int gridStep = 10;    // pixels
    static constexpr int gridX = 100;
    static constexpr int gridY = 30;
    static constexpr int gridCenterX = 570;
    static constexpr int gridCenterY = 400;
    static constexpr int spaceStep = 100;  // um
    static constexpr int widthStep = 100;
    static constexpr int maxX = 10000;
    static constexpr int maxY = 10000;
    static constexpr int minFontSize = 1;
    static constexpr int maxFontSize = 100;
    static constexpr int minSpace = 100;
    static constexpr int maxSpace = 100000;
    static constexpr int minTurningRadius = 1;
    static constexpr int maxTurningRadius = 1000000;
    static constexpr int minWidth = 100;
    static constexpr int maxWidth = 100000;
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
    int viaDiameter;
    int viaInnerDiameter;
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
