// schematiceditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SCHEMATICEDITOR_H
#define SCHEMATICEDITOR_H

#include "schematic.h"
#include "ui_schematiceditor.h"
#include <QJsonObject>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QToolButton>

const static char *schematicDirectory = "";
constexpr static int maxButton = 53;

enum ToolButton
{
    DECREASE_STEP, DELETE, DELETE_JUNCTION, DELETE_NET, DELETE_WIRE, ENUMERATE,
    HORIZONTAL_MIRROR, INCREASE_STEP, MOVE, MOVE_GROUP, MOVE_DOWN, MOVE_LEFT,
    MOVE_RIGHT, MOVE_UP, PLACE_BATTERY, PLACE_BUTTON, PLACE_CAPACITOR, PLACE_CELL,
    PLACE_CORE_INDUCTOR, PLACE_DEVICE, PLACE_DIODE, PLACE_DOUBLE_DIODE,
    PLACE_DOUBLE_SCHOTTKY, PLACE_FEMALE_CONNECTOR, PLACE_GROUND, PLACE_GROUND_IEC,
    PLACE_INDUCTOR, PLACE_JUNCTION, PLACE_LED, PLACE_MALE_CONNECTOR, PLACE_NET_NAME,
    PLACE_N_MOSFET, PLACE_NO_CONNECTION, PLACE_NPN_TRANSISTOR, PLACE_PHOTODIODE,
    PLACE_P_MOSFET, PLACE_PNP_TRANSISTOR, PLACE_POLAR_CAPACITOR, PLACE_POWER,
    PLACE_QUARTZ, PLACE_RESISTOR_ANSI, PLACE_RESISTOR_IEC, PLACE_SCHOTTKY,
    PLACE_SWITCH, PLACE_WIRE, PLACE_ZENER, SELECT, SELECT_PACKAGES, SET_VALUE,
    SHOW_NET_NUMBERS, TURN_TO_LEFT, TURN_TO_RIGHT, UPDATE_NETS
};

// elementType[ToolButton]
constexpr int elementType[maxButton] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BATTERY, BUTTON, CAPACITOR,
    CELL, CORE_INDUCTOR, 0, DIODE, 0, 0, 0, 0, 0, INDUCTOR, 0, LED, 0, 0, N_MOSFET,
    0, NPN_TRANSISTOR, PHOTODIODE, P_MOSFET, PNP_TRANSISTOR, POLAR_CAPACITOR, 0,
    QUARTZ, RESISTOR_ANSI, RESISTOR_IEC, SCHOTTKY, 0, 0, ZENER, 0, 0, 0, 0, 0, 0
};

class SchematicEditor : public QMainWindow, private Ui::SchematicEditor
{
    Q_OBJECT

public:
    explicit SchematicEditor(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);

private:
    // Set buttons: left, right, up, down, zoom in, zoom out
    // void buttonsSetEnabled(const char *params);
    void paintEvent(QPaintEvent *);
    void selectArray(int type, int &pins, int &orientation);
    void selectDevice(int &deviceNameID);
    void selectDiode(QString title, int &diodeTypeID);
    void selectPackages();
    // void writeLibraryFile(QString filename, QJsonObject object);

private slots:
    void about();
    void closeFile();
    void openFile();
    void saveComponentList();
    void saveErrorCheck();
    void saveFile();
    // void saveJSON();
    void saveNetlist();
    void saveSVG();
    void selectCommand(int);

private:
    int command;
    int dx, dy;
    int grid;
    int maxX, maxY;
    int orientation;
    int previousCommand;
    int step;
    QPoint mousePoint;
    QSignalMapper *signalMapper;
    QToolButton *toolButton[maxButton];
    Schematic schematic;
    std::vector<Line> lines;
};

#endif  // SCHEMATICEDITOR_H
