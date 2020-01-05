// pcbeditor.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "pcbeditor.h"
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

PcbEditor::PcbEditor(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    connect(actionNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionSaveErrorCheck, SIGNAL(triggered()), this, SLOT(saveErrorCheck()));
    connect(actionSaveSVG, SIGNAL(triggered()), this, SLOT(saveSVG()));
    connect(actionSaveJSON, SIGNAL(triggered()), this, SLOT(saveJSON()));
    connect(actionPackageEditor, SIGNAL(triggered()), this, SLOT(openPackageEditor()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QCheckBox *tmpCheckBox[checkBoxes] =
    {
        frontCheckBox, backCheckBox, borderCheckBox, padCheckBox,
        frontPolygonCheckBox, backPolygonCheckBox, frontViaCheckBox, backViaCheckBox,
        packageCheckBox, referenceCheckBox, nameCheckBox, showGridCheckBox,
        showNetsCheckBox
    };

    std::copy(tmpCheckBox, tmpCheckBox + checkBoxes, checkBox);

    for (int i = 0; i < checkBoxes; i++)
        connect(checkBox[i], &QCheckBox::clicked, [=] () { selectCheckBox(); });

    QPushButton *tmpPushButton[pushButtons] =
    {
        decGridButton, decSpaceButton, decWidthButton,
        incGridButton, incSpaceButton, incWidthButton
    };

    std::copy(tmpPushButton, tmpPushButton + pushButtons, pushButton);

    for (int i = 0; i < pushButtons; i++)
        connect(pushButton[i], &QPushButton::clicked, [=] () { selectPushButton(i); });

    QRadioButton *tmpRadioButton[radioButtons] =
    {
        readOnlyRadioButton, frontRadioButton, backRadioButton, boardRadioButton
    };

    std::copy(tmpRadioButton, tmpRadioButton + radioButtons, radioButton);

    for (int i = 0; i < radioButtons; i++)
        connect(radioButton[i], &QRadioButton::clicked, [=] () { selectRadioButton(); });

    QToolButton *tmpToolButton[toolButtons] =
    {
        createGroupsButton, decreaseStepButton, deleteButton, deleteJunctionButton,
        deletePolygonButton, deleteNetSegmentsButton, deleteSegmentButton, enumerateButton,
        fillPolygonButton, increaseStepButton, meterButton, moveButton,
        moveGroupButton, moveDownButton, moveLeftButton, moveRightButton,
        moveUpButton, placeElementsButton, placeInductorButton, placeInductor2Button,
        placeJunctionButton, placeLineButton, placeNoConnectionButton, placeNpnTransistorButton,
        placePnpTransistorButton, placePolygonButton, placePowerButton, placeQuartzButton,
        placeSegmentButton, placeShottkyButton, placeSwitchButton, placeZenerButton,
        routeTracksButton, segmentNetsButton, selectButton, setValueButton,
        showGroundNetsButton, tableRouteButton, turnToLeftButton, turnToRightButton,
        updateNetsButton, waveRouteButton, zoomInButton, zoomOutButton
    };

    std::copy(tmpToolButton, tmpToolButton + toolButtons, toolButton);

    for (int i = 0; i < toolButtons; i++)
        connect(toolButton[i], &QToolButton::clicked, [=] () { selectToolButton(i); });

    QString str;
    command = SELECT;
    previousCommand = command;
    mousePoint = QPoint(0, 0);    
    step = gridStep;
    gridNumber = 6; // 1000 um in grid step
    scale = double(gridStep) / grid[gridNumber];
    gridLineEdit->setText(str.setNum(grid[gridNumber]));
    space = board.defaultPolygonSpace;
    spaceLineEdit->setText(str.setNum(space));
    width = board.defaultLineWidth;
    widthLineEdit->setText(str.setNum(width));
    dx = gridX;
    dy = gridY;
    centerX = 57 * grid[gridNumber];
    centerY = 40 * grid[gridNumber];
    maxXLineEdit->setText(str.setNum(maxX));
    maxYLineEdit->setText(str.setNum(maxY));
    dxLineEdit->setText(str.setNum(dx));
    dyLineEdit->setText(str.setNum(dy));
    stepLineEdit->setText(str.setNum(step));
}

void PcbEditor::about()
{
    QMessageBox::information(this, tr("About PCB Editor"),
        tr("PCB Editor\n""Copyright (C) 2018 Alexander Karpeko"));
}

/*
void PcbEditor::buttonsSetEnabled(const char *params)
{
    for (int i = 0; i < maxButton; i++) {
        if (params[i] == '1')
            button[i]->setEnabled(true);
        if (params[i] == '0')
            button[i]->setEnabled(false);
    }
}
*/

void PcbEditor::closeFile()
{
    board.clear();

    actionOpenFile->setEnabled(true);
    actionCloseFile->setEnabled(false);

    update();

    // Left, right, up, down, zoom in, zoom out
    // buttonsSetEnabled("000000");
}

void PcbEditor::keyPressEvent(QKeyEvent *event)
{/*
    switch (command) {
    case PLACE_NET_NAME:
        if (board.selectedWire)
            if (event->text() != QString("q"))
                board.value += event->text();
            else
                board.addNetName(0, 0);
        break;
    case SET_VALUE:
        if (board.selectedElement)
            if (event->text() != QString("q"))
                board.value += event->text();
            else
                board.setValue(0, 0);
        break;
    }
*/
    update();
}

void PcbEditor::limit(int &value, int min, int max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
}

void PcbEditor::mousePressEvent(QMouseEvent *event)
{
    enum ElementOrientation {UP, RIGHT, DOWN, LEFT};

    int x;
    int y;
    QString str;

    if (event->button() == Qt::LeftButton) {
        mousePoint = event->pos();
        x = (1. / scale) * (gridStep * ((mousePoint.x() + gridStep / 2) / gridStep) - dx);
        y = (1. / scale) * (gridStep * ((mousePoint.y() + gridStep / 2) / gridStep) - dy);

        switch (command) {
       /* case DELETE:
            board.deleteElement(x, y);
            break;
        case DELETE_JUNCTION:
            board.deleteJunction(x, y);
            break;        
        case DELETE_WIRE:
            board.deleteWire(x, y);
            break;*/
        case DELETE_POLYGON:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER ||
                board.layers.edit == BORDER_LAYER)
                board.deletePolygon(x, y);
            break;
        case DELETE_NET_SEGMENTS:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
                board.deleteNetSegments(x, y);
            break;
        case DELETE_SEGMENT:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
                board.deleteSegment(x, y);
            break;
        case FILL_POLYGON:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
                board.fillPolygon(x, y);
            break;
        case METER:
            meterLineEdit->setText(str.setNum(board.meter(x, y), 'f', 2));
            break;
        case MOVE:
            board.moveElement(x, y);
            break;
        case MOVE_GROUP:
            board.moveGroup(x, y, scale);
            break;
        /*case PLACE_DEVICE:
            board.addDevice(board.deviceNameID, x, y);
            break;
        case PLACE_GROUND:
            board.addSymbol(GROUND, x, y);
            break;
        case PLACE_JUNCTION:
            board.addJunction(x, y);
            break;
        case PLACE_NET_NAME:
            board.addNetName(x, y);
            break; */
        case PLACE_LINE:
            break;
        case PLACE_POLYGON:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER ||
                board.layers.edit == BORDER_LAYER)
                board.points.push_back(Point(x, y));
            break;
        case PLACE_SEGMENT:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
                board.addSegmentPoint(x, y, width);
            break;
       // case SET_VALUE:
       //     board.setValue(x, y);
       //     break;
        case TURN_TO_LEFT:
            board.turnElement(x, y, LEFT);
            break;
        case TURN_TO_RIGHT:
            board.turnElement(x, y, RIGHT);
            break;
        }

        update();
    }

    if (event->button() == Qt::RightButton) {
        mousePoint = event->pos();
        x = (1. / scale) * (gridStep * ((mousePoint.x() + gridStep / 2) / gridStep) - dx);
        y = (1. / scale) * (gridStep * ((mousePoint.y() + gridStep / 2) / gridStep) - dy);

        switch (command) {
        case PLACE_LINE:
            break;
        case PLACE_POLYGON:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER ||
                board.layers.edit == BORDER_LAYER)
                board.addPolygon();
            break;
        case PLACE_SEGMENT:
            if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
                board.addTrack();
            break;
        default:
            command = SELECT;
            board.selectedElement = false;
            board.showMessage = false;
            board.pointNumber = 0;
        }

        update();
    }
}

void PcbEditor::newFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open netlist file"),
                       boardDirectory, tr("netlist files (*.net)"));
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
        board.fromNetlist(array);
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

void PcbEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open pcb file"),
                       boardDirectory, tr("pcb files (*.pcb)"));
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
        board.fromJson(array);
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

void PcbEditor::openPackageEditor()
{
    packageEditor.show();
}

void PcbEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont serifFont("Times", 10, QFont::Normal);
    QFont serifFont2("Times", 12, QFont::Normal);

    painter.fillRect(91, 0, 1160, 840, QColor(255, 255, 255, 255));

    painter.setPen(Qt::black);
    painter.setFont(serifFont);

    if (showGridCheckBox->isChecked()) {
        for (int i = 0; i < 1150 / gridStep; i++)
            for (int j = 0; j < 810 / gridStep; j++)
                painter.drawPoint(gridX + gridStep * i, gridY + gridStep * j);
    }

    // Draw message
    if (board.showMessage) {
        painter.setFont(serifFont2);
        painter.drawText(90, 890, board.message);
    }

    painter.translate(dx, dy);

    board.draw(painter, scale);
}

void PcbEditor::saveErrorCheck()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save erc file"),
                       boardDirectory, tr("erc files (*.erc)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    QString text;

    board.errorCheck(text);
    out << text;

    file.close();
}

void PcbEditor::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save pcb file"),
                          boardDirectory, tr("pcb files (*.pcb)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(board.toJson());
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void PcbEditor::saveSVG()
{
    constexpr int x = 0;
    constexpr int y = 0;
    constexpr int width = 1500;
    constexpr int height = 1000;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save svg file"),
                       boardDirectory, tr("svg files (*.svg)"));

    QSvgGenerator generator;
    generator.setFileName(fileName);
    generator.setSize(QSize(width, height));
    generator.setViewBox(QRect(x, y, width, height));
    generator.setTitle(tr("Board"));
    generator.setDescription(tr("Board"));
    QPainter painter;
    painter.begin(&generator);
    painter.fillRect(x, y, width, height, Qt::white);
    board.draw(painter, scale);
    painter.end();
}

void PcbEditor::saveJSON()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    writeLibraryFile("connectors.pkg", board.element.writePackages("Connector"));
    writeLibraryFile("devices.pkg", board.element.writePackages("Device"));
    writeLibraryFile("rcpackages.pkg", board.element.writePackages("RC"));
}

void PcbEditor::writeLibraryFile(QString filename, QJsonObject object)
{
    QFile file("library/json/" + filename);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonDocument document(object);
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void PcbEditor::selectCheckBox()
{
    for (int i = 0; i < checkBoxes; i++)
        if (checkBox[i]->isChecked())
            board.layers.draw |= (1 << i);
        else
            board.layers.draw &= ~(1 << i);

    if (showNetsCheckBox->isChecked())
        board.showNets = true;
    else
        board.showNets = false;

    update();
}

void PcbEditor::selectDevice(int &deviceNameID)
{
  /*QStringList deviceNames;
    for (DeviceSymbolIt i = Device::symbols.begin(); i != Device::symbols.end(); ++i)
        deviceNames += (*i).second.name;
    DeviceSelector deviceSelector(deviceNames);
    deviceNameID = deviceSelector.exec() - 1;
  */
}

void PcbEditor::selectPushButton(int number)
{
    int newGrid;
    QString str;

    switch (number) {
    case DEC_GRID:
        gridNumber -= 2;
    case INC_GRID:
        gridNumber++;
        limit(gridNumber, 0, grids - 1);
        newGrid = grid[gridNumber];
        scale = double(gridStep) / newGrid;
        dx = gridX + 570 - scale * centerX;
        dy = gridY + 400 - scale * centerY;
        dx = 10 * int(dx / 10 + 0.5);
        dy = 10 * int(dy / 10 + 0.5);
        gridLineEdit->setText(str.setNum(newGrid));
        dxLineEdit->setText(str.setNum(dx));
        dyLineEdit->setText(str.setNum(dy));
        break;
    case DEC_SPACE:
        space -= 2 * spaceStep;
    case INC_SPACE:
        space += spaceStep;
        limit(space, minSpace, maxSpace);
        spaceLineEdit->setText(str.setNum(space));
        break;
    case DEC_WIDTH:
        width -= 2 * widthStep;
    case INC_WIDTH:
        width += widthStep;
        limit(width, minWidth, maxWidth);
        widthLineEdit->setText(str.setNum(width));
        break;
    }

    update();
}

void PcbEditor::selectRadioButton()
{
    for (int i = 0; i < radioButtons; i++)
        if (radioButton[i]->isChecked())
            board.layers.edit = i - 1;
}

void PcbEditor::selectToolButton(int number)
{
    int tmp;
    QString str;

    previousCommand = command;
    command = number;

    switch (command) {
    case CREATE_GROUPS:
        board.createGroups();
        break;
    case DECREASE_STEP:
        step /= 4;
    case INCREASE_STEP:
        step *= 2;
        limit(step, gridStep, 16 * gridStep);
        stepLineEdit->setText(str.setNum(step));
        break;
    case METER:
        meterLineEdit->setText(str.setNum(0));
        break;
    case MOVE:
        board.selectedElement = false;
        break;
    case MOVE_DOWN:
        dy += 2 * step;
    case MOVE_UP:
        dy -= step;
        centerY = (gridY + 400 - dy) / scale + 0.5;
        dyLineEdit->setText(str.setNum(dy));
        break;
    case MOVE_GROUP:
        board.points.clear();
        break;
    case MOVE_LEFT:
        dx -= 2 * step;
    case MOVE_RIGHT:
        dx += step;
        centerX = (gridX + 570 - dx) / scale + 0.5;
        dxLineEdit->setText(str.setNum(dx));
        break;        
    /*case PLACE_DEVICE:
        selectDevice(board.deviceNameID);
        if (board.deviceNameID == -1)
            command = SELECT;
        break;
    case PLACE_NET_NAME:
        board.selectedWire = false;
        break;*/
    case PLACE_ELEMENTS:
        board.placeElements();
        break;
    case PLACE_LINE:
        break;
    case PLACE_POLYGON:
        if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER ||
            board.layers.edit == BORDER_LAYER)
            board.points.clear();
        break;
    case PLACE_SEGMENT:
        if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
            board.pointNumber = 0;
        break;
    case ROUTE_TRACKS:
        board.routeTracks();
        break;
    case SEGMENT_NETS:
        if (board.layers.edit == FRONT_LAYER || board.layers.edit == BACK_LAYER)
            board.segmentNets();
        break;
    case SELECT:
        board.selectedElement = false;
        board.showMessage = false;
        board.pointNumber = 0;
        break;
    /* case SET_VALUE:
        board.selectedElement = false;
        break; */
    case SHOW_GROUND_NETS:
        //board.showGroundNets = true;
        //std::cerr << "board.showGroundNets = true" << std::endl;
        board.showGroundNets ^= 1;
        break;
    /* case UPDATE_NETS:
        break; */
    case TABLE_ROUTE:
        board.tableRoute();
        break;
    case WAVE_ROUTE:
        stepLineEdit->setText(str.setNum(0));
        tmp = board.waveRoute();
        stepLineEdit->setText(str.setNum(tmp));
        break;
    case ZOOM_IN:
        gridNumber -= 2;
    case ZOOM_OUT:
        gridNumber++;
        limit(gridNumber, 0, grids - 1);
        scale = double(gridStep) / grid[gridNumber];
        gridLineEdit->setText(str.setNum(grid[gridNumber]));
        break;
    }

    update();
}
