// symboleditor.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
#include "symboleditor.h"
//#include "symbolselector.h"
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
#include <QPainter>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>

SymbolEditor::SymbolEditor(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actionNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(actionNewSymbol, SIGNAL(triggered()), this, SLOT(newSymbol()));
    connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionSaveAsFile, SIGNAL(triggered()), this, SLOT(saveAsFile()));
    connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
/*
    QToolButton *tmp[maxButton] =
    {
        decreaseStepButton, deleteButton, deleteJunctionButton, deleteNetButton,
        deleteWireButton, enumerateButton, increaseStepButton, moveButton,
        moveGroupButton, moveDownButton, moveLeftButton, moveRightButton,
        moveUpButton, placeBatteryButton, placeButtonButton, placeCapacitorButton,
        placeCellButton, placeCoreInductorButton, placeDeviceButton, placeDiodeButton,
        placeFemaleConnectorButton, placeGroundButton, placeInductorButton, placeJunctionButton,
        placeLEDButton, placeMaleConnectorButton, placeNetNameButton, placeNMosfetButton,
        placeNoConnectionButton, placeNpnTransistorButton, placePhotodiodeButton, placePMosfetButton,
        placePnpTransistorButton, placePolarCapacitorButton, placePowerButton, placeQuartzButton,
        placeResistorANSIButton, placeResistorIECButton, placeSchottkyButton, placeSwitchButton,
        placeWireButton, placeZenerButton, selectButton, selectPackagesButton,
        setValueButton, showNetNumbersButton, turnToLeftButton, turnToRightButton,
        updateNetsButton
    };

    std::copy(tmp, tmp + maxButton, toolButton);

    signalMapper = new QSignalMapper(this);
    for (int i = 0; i < maxButton; i++) {
        signalMapper->setMapping(toolButton[i], i);
        connect(toolButton[i], SIGNAL(clicked()), signalMapper, SLOT(map()));
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(selectCommand(int)));
*/
    command = SELECT;
    previousCommand = command;
    mousePoint = QPoint(0, 0);
    maxX = 10000;
    maxY = 10000;
    dx = 0;
    dy = 0;
    grid = 10;
    step = grid;
/*
    QString str;
    maxXLineEdit->setText(str.setNum(maxX/grid));
    maxYLineEdit->setText(str.setNum(maxY/grid));
    dxLineEdit->setText(str.setNum(dx/grid));
    dyLineEdit->setText(str.setNum(-dy/grid));
    stepLineEdit->setText(str.setNum(step/grid)); */
}

void SymbolEditor::about()
{
    QMessageBox::information(this, tr("About Symbol Editor"),
        tr("Symbol Editor\n""Copyright (C) 2018 Alexander Karpeko"));
}

void SymbolEditor::closeFile()
{
    //symbol.clear();

    actionCloseFile->setEnabled(false);
    actionNewFile->setEnabled(true);
    actionOpenFile->setEnabled(true);

    update();
}

void SymbolEditor::newFile()
{

}

void SymbolEditor::newSymbol()
{

}

void SymbolEditor::saveAsFile()
{

}

void SymbolEditor::saveFile()
{

}

/*
void SymbolEditor::keyPressEvent(QKeyEvent *event)
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

void SymbolEditor::mousePressEvent(QMouseEvent *event)
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
            schematic.addSymbol(GROUND, x, y);
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
            schematic.selectedElement = false;
            schematic.selectedSymbol = false;
        }

        update();
    }
}
*/

void SymbolEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open sym file"),
                       symbolDirectory, tr("sym files (*.sym)"));
    if (fileName.isNull()) {
        QMessageBox::warning(this, tr("Error"), tr("Filename is null"));
        return;
    }

    try {
        QByteArray byteArray;
        readJsonFile(fileName, byteArray);
        readSymbols(byteArray);
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(this, tr("Error"), e.show());
        return;
    }

    actionCloseFile->setEnabled(true);
    actionNewFile->setEnabled(false);
    actionOpenFile->setEnabled(false);

    update();
}

void SymbolEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont serifFont("Times", 10, QFont::Normal);
    const int turnings = 4;
    const int turnView[turnings][2] =
    {
        {900, 50}, {1050, 50}, {900, 200}, {1050, 200}
    };

    painter.fillRect(91, 20, 700, 500, QColor(Qt::white));

    for (int i = 0; i < turnings; i++)
        painter.fillRect(turnView[i][0], turnView[i][1],
                         120, 120, QColor(Qt::white));

    painter.setPen(Qt::black);
    painter.setFont(serifFont);

    for (int i = 100; i < 790; i += grid)
        for (int j = 30; j < 520; j += grid)
            painter.drawPoint(i, j);

    for (int i = 0; i < 4; i++)
        for (int j = turnView[i][0] + 10; j < turnView[i][0] + 120; j += grid)
            for (int k = turnView[i][1] + 10; k < turnView[i][1] + 120; k += grid)
                painter.drawPoint(j, k);

    painter.translate(dx, dy);

    //schematic.draw(painter);
}

void SymbolEditor::readJsonFile(const QString &filename, QByteArray &byteArray)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw ExceptionData(filename + " open error");
    byteArray = file.readAll();
    file.close();
}

void SymbolEditor::readSymbols(const QByteArray &byteArray)
{ /*
    QJsonDocument document(QJsonDocument::fromJson(byteArray));
    if (document.isNull())
        throw ExceptionData("Symbol file read error");

    QJsonObject object = document.object();

    if (object["object"].toString() != "symbols")
        throw ExceptionData("File is not a symbol file");

    if (object.contains("arrays")) {
        QJsonArray arraySymbols(object["arrays"].toArray());
        for (auto a : arraySymbols)
            Array::addSymbol(a);
         return;
    }

    if (object.contains("devices")) {
        QJsonArray deviceSymbols(object["devices"].toArray());
        for (auto d : deviceSymbols)
            Device::addSymbol(d);
        return;
    }

    if (object.contains("elements")) {
        QJsonArray elementSymbols(object["elements"].toArray());
        for (auto e : elementSymbols)
            Element::addSymbol(e);
        return;
    } */
}

/*
void SymbolEditor::saveComponentList()
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

void SymbolEditor::saveFile()
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
*/

/*
void SymbolEditor::saveJSON()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    writeLibraryFile("arrays.sym", schematic.array.writeSymbols());
    writeLibraryFile("devices.sym", schematic.device.writeSymbols());
    writeLibraryFile("elements.sym", schematic.element.writeSymbols());
}
*/

/*
void SymbolEditor::selectArray(int type, int &pins, int &orientation)
{
    QString titles[3] = {"Connector Selector", "Connector Selector", "Switch Selector"};

    ArraySelector arraySelector(titles[type]);

    int n = arraySelector.exec();
    pins = n >> 1;
    orientation = n & 1;
}

void SymbolEditor::selectCommand(int number)
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
        schematic.selectedDevice = false;
        schematic.selectedElement = false;
        schematic.selectedSymbol = false;
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

void SymbolEditor::selectDevice(int &deviceNameID)
{
    QStringList deviceNames;
    for (auto ds : Device::symbols)
        deviceNames += ds.second.name;
    DeviceSelector deviceSelector(deviceNames);
    deviceNameID = deviceSelector.exec() - 1;
}

void SymbolEditor::selectPackages()
{
    PackageSelector packageSelector(schematic);
    packageSelector.exec();
}
*/

/*
void SymbolEditor::writeLibraryFile(QString filename, QJsonObject object)
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
