// symboleditor.h
// Copyright (C) 2018 Alexander Karpeko

#ifndef SYMBOLEDITOR_H
#define SYMBOLEDITOR_H

#include "array.h"
#include "device.h"
#include "element.h"
#include "types.h"
#include "ui_symboleditor.h"
#include <QByteArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QString>
#include <QToolButton>

const static char *symbolDirectory = "";
const static int maxButton = 49;

enum ToolButton
{
    DECREASE_STEP, DELETE, DELETE_JUNCTION, DELETE_NET,
    DELETE_WIRE, ENUMERATE, INCREASE_STEP, MOVE,
    MOVE_GROUP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT,
    MOVE_UP, PLACE_BATTERY, PLACE_BUTTON, PLACE_CAPACITOR,
    PLACE_CELL, PLACE_CORE_INDUCTOR, PLACE_DEVICE, PLACE_DIODE,
    PLACE_FEMALE_CONNECTOR, PLACE_GROUND, PLACE_INDUCTOR, PLACE_JUNCTION,
    PLACE_LED, PLACE_MALE_CONNECTOR, PLACE_NET_NAME, PLACE_N_MOSFET,
    PLACE_NO_CONNECTION, PLACE_NPN_TRANSISTOR, PLACE_PHOTODIODE, PLACE_P_MOSFET,
    PLACE_PNP_TRANSISTOR, PLACE_POLAR_CAPACITOR, PLACE_POWER, PLACE_QUARTZ,
    PLACE_RESISTOR_ANSI, PLACE_RESISTOR_IEC, PLACE_SCHOTTKY, PLACE_SWITCH,
    PLACE_WIRE, PLACE_ZENER, SELECT, SELECT_PACKAGES,
    SET_VALUE, SHOW_NET_NUMBERS, TURN_TO_LEFT, TURN_TO_RIGHT,
    UPDATE_NETS
};
/*
// elementType[ToolButton]
const int elementType[maxButton] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BATTERY, BUTTON, CAPACITOR,
    CELL, CORE_INDUCTOR, 0, DIODE, 0, 0, INDUCTOR, 0, LED, 0, 0, N_MOSFET,
    0, NPN_TRANSISTOR, PHOTODIODE, P_MOSFET, PNP_TRANSISTOR, POLAR_CAPACITOR, 0,
    QUARTZ, RESISTOR_ANSI, RESISTOR_IEC, SCHOTTKY, 0, 0, ZENER, 0, 0, 0, 0, 0, 0
};
*/
class SymbolEditor : public QMainWindow, private Ui::SymbolEditor
{
    Q_OBJECT

public:
    explicit SymbolEditor(QWidget *parent = 0);

protected:
    //void keyPressEvent(QKeyEvent *event);
    //void mousePressEvent(QMouseEvent *event);

private:
    void paintEvent(QPaintEvent *);
    void readJsonFile(const QString &filename, QByteArray &byteArray);
    void readSymbols(const QByteArray &byteArray);
    void selectSymbol(int &symbolNameID);

private slots:
    void about();
    void closeFile();
    void newFile();
    void newSymbol();
    void openFile();
    void saveAsFile();
    void saveFile();

private:
    int command;
    int dx, dy;
    int grid;
    int maxX, maxY;
    int orientation;
    int previousCommand;
    int step;
    QPoint mousePoint;
    //QSignalMapper *signalMapper;
    //QToolButton *toolButton[maxButton];
    //Symbol symbol;
    //std::vector<Line> lines;
};

#endif  // SYMBOLEDITOR_H
