// schematiceditor.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "arrayselector.h"
#include "deviceselector.h"
#include "exceptiondata.h"
#include "function.h"
#include "packageselector.h"
#include "schematiceditor.h"
#include <algorithm>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSvgGenerator>
#include <QTextStream>
#include <QVBoxLayout>

SchematicEditor::SchematicEditor(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionSaveComponentList, SIGNAL(triggered()), this, SLOT(saveComponentList()));
    connect(actionSaveErrorCheck, SIGNAL(triggered()), this, SLOT(saveErrorCheck()));
    connect(actionSaveNetlist, SIGNAL(triggered()), this, SLOT(saveNetlist()));
    connect(actionSaveSVG, SIGNAL(triggered()), this, SLOT(saveSVG()));
    connect(actionSaveJSON, SIGNAL(triggered()), this, SLOT(saveJSON()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QToolButton *tmp[maxButton] =
    {
        decreaseStepButton, deleteButton, deleteJunctionButton, deleteNetButton,
        deleteWireButton, enumerateButton, increaseStepButton, moveButton,
        moveGroupButton, moveDownButton, moveLeftButton, moveRightButton,
        moveUpButton, placeBatteryButton, placeButtonButton, placeCapacitorButton,
        placeCellButton, placeCoreInductorButton, placeDeviceButton, placeDiodeButton,
        placeFemaleConnectorButton, placeGroundButton, placeInductorButton, placeJunctionButton,
        placeLEDButton, placeMaleConnectorButton, placeNetNameButton, placeNoConnectionButton,
        placeNpnTransistorButton, placePhotodiodeButton, placePnpTransistorButton,
        placePolarCapacitorButton, placePowerButton, placeQuartzButton, placeResistorANSIButton,
        placeResistorIECButton, placeShottkyButton, placeSwitchButton, placeWireButton, placeZenerButton,
        selectButton, selectPackagesButton, setValueButton, showNetNumbersButton, turnToLeftButton,
        turnToRightButton, updateNetsButton
    };

    std::copy(tmp, tmp + maxButton, toolButton);

    signalMapper = new QSignalMapper(this);
    for (int i = 0; i < maxButton; i++) {
        signalMapper->setMapping(toolButton[i], i);
        connect(toolButton[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(selectCommand(int)));

    command = SELECT;
    previousCommand = command;
    mousePoint = QPoint(0, 0);
    maxX = 10000;
    maxY = 10000;
    dx = 0;
    dy = 0;
    grid = 10;
    step = grid;

    QString str;
    maxXLineEdit->setText(str.setNum(maxX/grid));
    maxYLineEdit->setText(str.setNum(maxY/grid));
    dxLineEdit->setText(str.setNum(dx/grid));
    dyLineEdit->setText(str.setNum(-dy/grid));
    stepLineEdit->setText(str.setNum(step/grid));
}
    
void SchematicEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont serifFont("Times", 10, QFont::Normal);
    QString str;

    painter.fillRect(91, 0, 1160, 840, QColor(255, 255, 255, 255));

    painter.setPen(Qt::black);
    painter.setFont(serifFont);

    int gridX = 100;
    int gridY = 40;

    for (int i = 0; i < 1150 / grid; i++)
        for (int j = 0; j < 800 / grid; j++)
            painter.drawPoint(gridX + grid * i, gridY + grid * j);

    painter.translate(dx, dy);

    schema.draw(painter);
}

void SchematicEditor::selectArray(int type, int &pins, int &orientation)
{
    QString titles[3] = {"Connector Selector", "Connector Selector", "Switch Selector"};

    ArraySelector arraySelector(titles[type]);

    int n = arraySelector.exec();
    pins = n >> 1;
    orientation = n & 1;
}

void SchematicEditor::selectDevice(int &deviceNameID)
{
    QStringList deviceNames;
    for (auto ds : Device::symbols)
        deviceNames += ds.second.name;
    DeviceSelector deviceSelector(deviceNames);
    deviceNameID = deviceSelector.exec() - 1;
}

void SchematicEditor::selectPackages()
{
    PackageSelector packageSelector(schema);
    packageSelector.exec();
}

/*
void SchematicEditor::buttonsSetEnabled(const char *params)
{
    for (int i = 0; i < maxButton; i++) {
        if (params[i] == '1')
            button[i]->setEnabled(true);
        if (params[i] == '0')
            button[i]->setEnabled(false);
    }
}
*/

void SchematicEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open sch file"),
                       schemaDirectory, tr("sch files (*.sch)"));
    if (fileName.isNull()) {
        QMessageBox::warning(this, tr("Error"), tr("Filename is null"));
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray array = file.readAll();
    file.close();

    try {
        schema.fromJson(array);
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(this, tr("Error"), e.show());
        return;
    }

    actionOpenFile->setEnabled(false);
    actionCloseFile->setEnabled(true);

    // Left, right, up, down, zoom in, zoom out
    // buttonsSetEnabled("111111");

    update();
}

void SchematicEditor::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save sch file"),
                       schemaDirectory, tr("sch files (*.sch)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(schema.toJson());
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void SchematicEditor::closeFile()
{
    schema.clear();

    actionOpenFile->setEnabled(true);
    actionCloseFile->setEnabled(false);

    // Left, right, up, down, zoom in, zoom out
    // buttonsSetEnabled("000000");

    update();
}

void SchematicEditor::saveComponentList()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save lst file"),
                       schemaDirectory, tr("lst files (*.lst)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    QString text;

    schema.componentList(text);
    out << text;

    file.close();
}

void SchematicEditor::saveErrorCheck()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save erc file"),
                       schemaDirectory, tr("erc files (*.erc)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    QString text;

    schema.errorCheck(text);
    out << text;

    file.close();
}

void SchematicEditor::saveNetlist()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save net file"),
                       schemaDirectory, tr("net files (*.net)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(schema.netlist());
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void SchematicEditor::saveSVG()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save svg file"),
                       schemaDirectory, tr("svg files (*.svg)"));
    QSvgGenerator generator;
    generator.setFileName(fileName);
    generator.setSize(QSize(1000, 1000));
    generator.setViewBox(QRect(0, 0, 1000, 1000));
    generator.setTitle(tr("Schema"));
    generator.setDescription(tr("Schema"));
    QPainter painter;
    painter.begin(&generator);
    schema.draw(painter);
    painter.end();
}

void SchematicEditor::saveJSON()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    writeLibraryFile("arrays.sym", schema.array.writeSymbols());
    writeLibraryFile("devices.sym", schema.device.writeSymbols());
    writeLibraryFile("elements.sym", schema.element.writeSymbols());
}

void SchematicEditor::writeLibraryFile(QString filename, QJsonObject object)
{
    QFile file("library/json/" + filename);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(object);
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void SchematicEditor::about()
{
    QMessageBox::information(this, tr("About Schematic Editor"),
        tr("Schematic Editor\n""Copyright (C) 2018 Alexander Karpeko"));
}

void SchematicEditor::selectCommand(int number)
{
    QString str;

    previousCommand = command;
    command = number;

    if (command >= PLACE_BATTERY && command <= PLACE_ZENER)
        orientation = UP;

    switch (command) {
    case DECREASE_STEP:
        step /= 2;
        if (step < grid)
            step = grid;
        stepLineEdit->setText(str.setNum(step/grid));
        break;
    case ENUMERATE:
        schema.enumerate();
        break;
    case INCREASE_STEP:
        step *= 2;
        if (step > 16 * grid)
            step = 16 * grid;
        stepLineEdit->setText(str.setNum(step/grid));
        break;
    case MOVE:
        schema.selectedArray = false;
        schema.selectedDevice = false;
        schema.selectedElement = false;
        schema.selectedSymbol = false;
        break;
    case MOVE_DOWN:
        dy += step;
        dyLineEdit->setText(str.setNum(-dy/grid));
        break;
    case MOVE_GROUP:
        schema.points.clear();
        break;
    case MOVE_LEFT:
        dx -= step;
        dxLineEdit->setText(str.setNum(dx/grid));
        break;
    case MOVE_RIGHT:
        dx += step;
        dxLineEdit->setText(str.setNum(dx/grid));
        break;
    case MOVE_UP:
        dy -= step;
        dyLineEdit->setText(str.setNum(-dy/grid));
        break;
    case PLACE_DEVICE:
        selectDevice(schema.deviceNameID);
        if (schema.deviceNameID == -1)
            command = SELECT;
        break;
    case PLACE_FEMALE_CONNECTOR:
        selectArray(ARRAY_FCON, schema.arrayNumber, schema.arrayOrientation);
        if (!schema.arrayNumber)
            command = SELECT;
        break;
    case PLACE_MALE_CONNECTOR:
        selectArray(ARRAY_MCON, schema.arrayNumber, schema.arrayOrientation);
        if (!schema.arrayNumber)
            command = SELECT;
        break;
    case PLACE_NET_NAME:
        schema.selectedWire = false;
        break;
    case PLACE_SWITCH:
        selectArray(ARRAY_SW, schema.arrayNumber, schema.arrayOrientation);
        if (!schema.arrayNumber)
            command = SELECT;
        break;
    case PLACE_WIRE:
        schema.pointNumber = 0;
        break;
    case SELECT_PACKAGES:
        selectPackages();
        command = SELECT;
        break;
    case SET_VALUE:
        schema.selectedArray = false;
        schema.selectedElement = false;
        break;
    case SHOW_NET_NUMBERS:
        schema.showNetNumbers ^= true;
        break;
    case TURN_TO_LEFT:
        if (previousCommand >= PLACE_BATTERY &&
            previousCommand <= PLACE_ZENER) {
            command = previousCommand;
            orientation--;
            if (orientation < 0)
                orientation = 3;
        }
        break;
    case TURN_TO_RIGHT:
        if (previousCommand >= PLACE_BATTERY &&
            previousCommand <= PLACE_ZENER) {
            command = previousCommand;
            orientation++;
            if (orientation > 3)
                orientation = 0;
        }
        break;    
    case UPDATE_NETS:
        schema.updateNets();
        break;    
    }

    update();
}

void SchematicEditor::keyPressEvent(QKeyEvent *event)
{
    switch (command) {    
    case PLACE_NET_NAME:
        if (schema.selectedWire) {
            if (event->text() != QString("q"))
                schema.value += event->text();
            else
                schema.addNetName(0, 0);
        }
        break;
    case SET_VALUE:
        if (schema.selectedArray || schema.selectedElement) {
            if (event->text() != QString("q"))
                schema.value += event->text();
            else
                schema.setValue(0, 0);
        }
        break;
    }

    update();
}

void SchematicEditor::mousePressEvent(QMouseEvent *event)
{
    int x;
    int y;

    if (event->button() == Qt::LeftButton) {
        mousePoint = event->pos();
        x = grid * ((mousePoint.x() + grid / 2) / grid);
        y = grid * ((mousePoint.y() + grid / 2) / grid);
        limit(x, 0, 10000);
        limit(y, 0, 10000);

        if (command >= PLACE_BATTERY && command <= PLACE_ZENER)
            schema.addElement(elementType[command], x, y, orientation);

        switch (command) {
        case DELETE:
            schema.deleteElement(x, y);
            break;
        case DELETE_JUNCTION:
            schema.deleteJunction(x, y);
            break;
        case DELETE_NET:
            schema.deleteNet(x, y);
            break;
        case DELETE_WIRE:
            schema.deleteWire(x, y);
            break;
        case MOVE:
            schema.move(x, y);
            break;
        case MOVE_GROUP:
            schema.moveGroup(x, y);
            break;
        case PLACE_DEVICE:
            schema.addDevice(schema.deviceNameID, x, y);
            break;
        case PLACE_FEMALE_CONNECTOR:
            schema.addArray(ARRAY_FCON, schema.arrayNumber, x, y, schema.arrayOrientation);
            break;
        case PLACE_GROUND:
            schema.addSymbol(GROUND, x, y);
            break;        
        case PLACE_JUNCTION:
            schema.addJunction(x, y);
            break;
        case PLACE_MALE_CONNECTOR:
            schema.addArray(ARRAY_MCON, schema.arrayNumber, x, y, schema.arrayOrientation);
            break;
        case PLACE_NET_NAME:
            schema.addNetName(x, y);
            break;
        case PLACE_SWITCH:
            schema.addArray(ARRAY_SW, schema.arrayNumber, x, y, schema.arrayOrientation);
            break;
        case PLACE_WIRE:
            schema.addPoint(x, y);
            break;        
        case SET_VALUE:
            schema.setValue(x, y);
            break;
        }

        update();
    }

    if (event->button() == Qt::RightButton) {
        mousePoint = event->pos();
        x = grid * ((mousePoint.x() + grid / 2) / grid);
        y = grid * ((mousePoint.y() + grid / 2) / grid);
        limit(x, 0, 10000);
        limit(y, 0, 10000);

        switch (command) {
        case PLACE_WIRE:
            schema.addNet();
            break;        
        default:
            command = SELECT;
            schema.selectedArray = false;
            schema.selectedElement = false;
            schema.selectedSymbol = false;
        }

        update();
    }
}
