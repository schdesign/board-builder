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
    // connect(actionSaveJSON, SIGNAL(triggered()), this, SLOT(saveJSON()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QToolButton *tmp[maxButton] =
    {
        decreaseStepButton, deleteButton, deleteJunctionButton, deleteNetButton,
        deleteWireButton, enumerateButton, increaseStepButton, moveButton,
        moveGroupButton, moveDownButton, moveLeftButton, moveRightButton,
        moveUpButton, placeBatteryButton, placeButtonButton, placeCapacitorButton,
        placeCellButton, placeCoreInductorButton, placeDeviceButton, placeDiodeButton,
        placeFemaleConnectorButton, placeGroundButton, placeGroundIECButton, placeInductorButton,
        placeJunctionButton, placeLEDButton, placeMaleConnectorButton, placeNetNameButton,
        placeNMosfetButton, placeNoConnectionButton, placeNpnTransistorButton, placePhotodiodeButton,
        placePMosfetButton, placePnpTransistorButton, placePolarCapacitorButton, placePowerButton,
        placeQuartzButton, placeResistorANSIButton, placeResistorIECButton, placeSchottkyButton,
        placeSwitchButton, placeWireButton, placeZenerButton, selectButton,
        selectPackagesButton, setValueButton, showNetNumbersButton, turnToLeftButton,
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

void SchematicEditor::about()
{
    QMessageBox::information(this, tr("About Schematic Editor"),
        tr("Schematic Editor\n""Copyright (C) 2018 Alexander Karpeko"));
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

void SchematicEditor::closeFile()
{
    schematic.clear();

    actionOpenFile->setEnabled(true);
    actionCloseFile->setEnabled(false);

    // Left, right, up, down, zoom in, zoom out
    // buttonsSetEnabled("000000");

    update();
}

void SchematicEditor::keyPressEvent(QKeyEvent *event)
{
    switch (command) {
    case PLACE_NET_NAME:
        if (schematic.selectedWire) {
            if (event->text() != QString("q"))
                schematic.value += event->text();
            else
                schematic.addNetName(0, 0);
        }
        break;
    case SET_VALUE:
        if (schematic.selectedArray || schematic.selectedElement) {
            if (event->text() != QString("q"))
                schematic.value += event->text();
            else
                schematic.setValue(0, 0);
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
        x = mousePoint.x();
        y = mousePoint.y();

        if (command == DELETE)
            schematic.deleteElement(x, y);
        if (command == SET_VALUE)
            schematic.setValue(x, y);

        // Set grid for x, y
        x = grid * ((x + grid / 2) / grid);
        y = grid * ((y + grid / 2) / grid);
        limit(x, 0, 10000);
        limit(y, 0, 10000);

        if (command >= PLACE_BATTERY && command <= PLACE_ZENER)
            schematic.addElement(elementType[command], x, y, orientation);

        switch (command) {
        case DELETE_JUNCTION:
            schematic.deleteJunction(x, y);
            break;
        case DELETE_NET:
            schematic.deleteNet(x, y);
            break;
        case DELETE_WIRE:
            schematic.deleteWire(x, y);
            break;
        case MOVE:
            schematic.move(x, y);
            break;
        case MOVE_GROUP:
            schematic.moveGroup(x, y);
            break;
        case PLACE_DEVICE:
            schematic.addDevice(schematic.deviceNameID, x, y);
            break;
        case PLACE_FEMALE_CONNECTOR:
            schematic.addArray(ARRAY_FCON, schematic.arrayNumber, x, y, schematic.arrayOrientation);
            break;
        case PLACE_GROUND:
            schematic.addCircuitSymbol(GROUND, x, y);
            break;
        case PLACE_GROUND_IEC:
            schematic.addCircuitSymbol(GROUND_IEC, x, y);
            break;
        case PLACE_JUNCTION:
            schematic.addJunction(x, y);
            break;
        case PLACE_MALE_CONNECTOR:
            schematic.addArray(ARRAY_MCON, schematic.arrayNumber, x, y, schematic.arrayOrientation);
            break;
        case PLACE_NET_NAME:
            schematic.addNetName(x, y);
            break;
        case PLACE_SWITCH:
            schematic.addArray(ARRAY_SW, schematic.arrayNumber, x, y, schematic.arrayOrientation);
            break;
        case PLACE_WIRE:
            schematic.addPoint(x, y);
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
            schematic.addNet();
            break;
        default:
            command = SELECT;
            schematic.selectedArray = false;
            schematic.selectedCircuitSymbol = false;
            schematic.selectedElement = false;
        }

        update();
    }
}

void SchematicEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open sch file"),
                       schematicDirectory, tr("sch files (*.sch)"));
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
        schematic.fromJson(array);
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

    schematic.draw(painter);
}

void SchematicEditor::saveComponentList()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save lst file"),
                       schematicDirectory, tr("lst files (*.lst)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    QString text;

    schematic.componentList(text);
    out << text;

    file.close();
}

void SchematicEditor::saveErrorCheck()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save erc file"),
                       schematicDirectory, tr("erc files (*.erc)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    QString text;

    schematic.errorCheck(text);
    out << text;

    file.close();
}

void SchematicEditor::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save sch file"),
                       schematicDirectory, tr("sch files (*.sch)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(schematic.toJson());
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}
/*
void SchematicEditor::saveJSON()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    writeLibraryFile("arrays.sym", schematic.array.writeSymbols());
    writeLibraryFile("devices.sym", schematic.device.writeSymbols());
    writeLibraryFile("elements.sym", schematic.element.writeSymbols());
}
*/
void SchematicEditor::saveNetlist()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save net file"),
                       schematicDirectory, tr("net files (*.net)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(schematic.netlist());
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void SchematicEditor::saveSVG()
{
    constexpr int x = 0;
    constexpr int y = 0;
    constexpr int width = 1500;
    constexpr int height = 1000;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save svg file"),
                       schematicDirectory, tr("svg files (*.svg)"));

    QSvgGenerator generator;
    generator.setFileName(fileName);
    generator.setSize(QSize(width, height));
    generator.setViewBox(QRect(x, y, width, height));
    generator.setTitle(tr("Schematic"));
    generator.setDescription(tr("Schematic"));

    QPainter painter;
    painter.begin(&generator);
    painter.fillRect(x, y, width, height, Qt::white);
    schematic.draw(painter);
    painter.end();
}

void SchematicEditor::selectArray(int type, int &pins, int &orientation)
{
    QString titles[3] = {"Connector Selector", "Connector Selector", "Switch Selector"};

    ArraySelector arraySelector(titles[type]);

    int n = arraySelector.exec();
    pins = n >> 1;
    orientation = n & 1;
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
        schematic.enumerate();
        break;
    case INCREASE_STEP:
        step *= 2;
        if (step > 16 * grid)
            step = 16 * grid;
        stepLineEdit->setText(str.setNum(step/grid));
        break;
    case MOVE:
        schematic.selectedArray = false;
        schematic.selectedCircuitSymbol = false;
        schematic.selectedDevice = false;
        schematic.selectedElement = false;
        break;
    case MOVE_DOWN:
        dy += step;
        dyLineEdit->setText(str.setNum(-dy/grid));
        break;
    case MOVE_GROUP:
        schematic.points.clear();
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
        selectDevice(schematic.deviceNameID);
        if (schematic.deviceNameID == -1)
            command = SELECT;
        break;
    case PLACE_FEMALE_CONNECTOR:
        selectArray(ARRAY_FCON, schematic.arrayNumber, schematic.arrayOrientation);
        if (!schematic.arrayNumber)
            command = SELECT;
        break;
    case PLACE_MALE_CONNECTOR:
        selectArray(ARRAY_MCON, schematic.arrayNumber, schematic.arrayOrientation);
        if (!schematic.arrayNumber)
            command = SELECT;
        break;
    case PLACE_NET_NAME:
        schematic.selectedWire = false;
        break;
    case PLACE_SWITCH:
        selectArray(ARRAY_SW, schematic.arrayNumber, schematic.arrayOrientation);
        if (!schematic.arrayNumber)
            command = SELECT;
        break;
    case PLACE_WIRE:
        schematic.pointNumber = 0;
        break;
    case SELECT_PACKAGES:
        selectPackages();
        command = SELECT;
        break;
    case SET_VALUE:
        schematic.selectedArray = false;
        schematic.selectedElement = false;
        break;
    case SHOW_NET_NUMBERS:
        schematic.showNetNumbers ^= true;
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
        schematic.updateNets();
        break;    
    }

    update();
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
    PackageSelector packageSelector(schematic);
    packageSelector.exec();
}

/*
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
*/
