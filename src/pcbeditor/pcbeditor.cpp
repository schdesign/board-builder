// pcbeditor.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "elementoptions.h"
#include "exceptiondata.h"
#include "function.h"
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
    connect(actionElementOptions, SIGNAL(triggered()), this, SLOT(elementOptions()));
    connect(actionPackageEditor, SIGNAL(triggered()), this, SLOT(openPackageEditor()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QCheckBox *tmpCheckBox[checkBoxes] =
    {
        fillPadsCheckBox, showGridCheckBox, showNetsCheckBox
    };

    std::copy(tmpCheckBox, tmpCheckBox + checkBoxes, checkBox);

    for (int i = 0; i < checkBoxes; i++)
        connect(checkBox[i], &QCheckBox::clicked, [=] () { selectCheckBox(i); });

    QCheckBox *tmpLayerCheckBox[layersNumber] =
    {
        frontCheckBox, backCheckBox, borderCheckBox, padCheckBox, frontPolygonCheckBox,
        backPolygonCheckBox, frontViaCheckBox, backViaCheckBox, packageCheckBox,
        referenceCheckBox, nameCheckBox
    };

    std::copy(tmpLayerCheckBox, tmpLayerCheckBox + layersNumber, layerCheckBox);

    for (int i = 0; i < layersNumber; i++)
        connect(layerCheckBox[i], &QCheckBox::clicked, [=] () { selectLayerCheckBox(); });

    QPushButton *tmpPushButton[pushButtons] =
    {
        decFontSizePushButton, decGridPushButton, decSpacePushButton, decWidthPushButton,
        incFontSizePushButton, incGridPushButton, incSpacePushButton, incWidthPushButton,
        turningRadiusPushButton
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
        connectJumperButton, createGroupsButton, decreaseStepButton, deleteButton,
        deleteJumperButton, deleteJunctionButton, deleteNetSegmentsButton,
        deletePolygonButton, deleteSegmentButton, disconnectJumperButton, fillPolygonButton,
        increaseStepButton, meterButton, moveButton, moveDownButton, moveGroupButton,
        moveLeftButton, moveRightButton, moveUpButton, noRoundTurnButton, placeElementsButton,
        placeJumperButton, placeJunctionButton, placeLineButton, placePolygonButton,
        placeSegmentButton, roundTurnButton, routeTracksButton, segmentNetsButton, selectButton,
        setValueButton, showGroundNetsButton, tableRouteButton, turnToLeftButton,
        turnToRightButton, updateNetsButton, waveRouteButton, zoomInButton, zoomOutButton
    };

    std::copy(tmpToolButton, tmpToolButton + toolButtons, toolButton);

    for (int i = 0; i < toolButtons; i++)
        connect(toolButton[i], &QToolButton::clicked, [=] () { selectToolButton(i); });

    QString str;
    command = SELECT;
    previousCommand = command;
    mousePoint = QPoint(0, 0);
    fontSize = defaultFontSize;
    fontSizeLineEdit->setText(str.setNum(fontSize));
    step = gridStep;
    gridNumber = defaultGridNumber;
    scale = double(gridStep) / grid[gridNumber];
    gridLineEdit->setText(str.setNum(grid[gridNumber]));
    space = board.defaultPolygonSpace;
    spaceLineEdit->setText(str.setNum(space));
    turningRadius = defaultTurningRadius;
    turningRadiusLineEdit->setText(str.setNum(turningRadius));
    width = board.defaultLineWidth;
    widthLineEdit->setText(str.setNum(width));
    dx = gridX;
    dy = gridY;
    centerX = 57 * grid[gridNumber];
    centerY = 39 * grid[gridNumber];
    maxXLineEdit->setText(str.setNum(maxX));
    maxYLineEdit->setText(str.setNum(maxY));
    dxLineEdit->setText(str.setNum(dx));
    dyLineEdit->setText(str.setNum(dy));
    stepLineEdit->setText(str.setNum(step));

    showGrid = true;
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

void PcbEditor::elementOptions()
{
    ElementOptionsData options;
    options.padCornerRadius = Element::padCornerRadius;

    ElementOptions elementOptions(options);
    int n = elementOptions.exec();

    if (n == QDialog::Accepted) {
        Element::padCornerRadius = options.padCornerRadius;
        for (auto &e : board.elements)
            e.roundPadCorners();
        update();
    }
}

void PcbEditor::keyPressEvent(QKeyEvent *event)
{
/*  switch (command) {
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

    update(); */
}

void PcbEditor::mousePressEvent(QMouseEvent *event)
{
    enum ElementOrientation {UP, RIGHT, DOWN, LEFT};

    bool isChanged = true;
    int mpx, mpy;
    int x, y;
    QString str;

    bool isElementLayer = board.layers.edit == FRONT_LAYER ||
                          board.layers.edit == BACK_LAYER;
    bool isDrawingLayer = isElementLayer || board.layers.edit == BORDER_LAYER;

    if (event->button() == Qt::LeftButton) {
        mousePoint = event->pos();
        mpx = (1. / scale) * (mousePoint.x() - dx);
        mpy = (1. / scale) * (mousePoint.y() - dy);
        x = (1. / scale) * (gridStep * ((mousePoint.x() + gridStep / 2 ) / gridStep) - dx);
        y = (1. / scale) * (gridStep * ((mousePoint.y() + gridStep / 2 ) / gridStep) - dy);

        switch (command) {
        case METER:
            meterLineEdit->setText(str.setNum(board.meter(x, y), 'f', 2));
            break;
        }

        if (!isDrawingLayer)
            return;

        switch (command) {
        case DELETE_POLYGON:
            board.deletePolygon(x, y);
            break;
        case PLACE_LINE:
            break;
        case PLACE_POLYGON:
            if (isDrawingLayer)
                board.points.push_back(Point(x, y));
            break;
        default:
            isChanged = false;
        }

        if (isChanged) {
            update();
            return;
        }

        isChanged = true;

        if (!isElementLayer)
            return;

        switch (command) {
        case CONNECT_JUMPER:
            board.connectJumper(x, y);
            break;
        case DELETE_JUMPER:
            board.deleteJumper(x, y);
            break;
        case DELETE_NET_SEGMENTS:
            board.deleteNetSegments(x, y);
            break;
        case DELETE_SEGMENT:
            board.deleteSegment(x, y);
            break;
        case DISCONNECT_JUMPER:
            board.disconnectJumper(x, y);
            break;
        case FILL_POLYGON:
            board.fillPolygon(x, y);
            break;
        case MOVE:
            board.moveElement(x, y);
            break;
        case MOVE_GROUP:
            board.moveGroup(x, y, scale);
            break;
        case NO_ROUND_TURN:
            board.noRoundTurn(x, y);
            break;
        case PLACE_JUMPER:
            board.placeJumper(x, y);
            break;
        case PLACE_SEGMENT:
            board.addSegmentPoint(x, y, width);
            break;
        case ROUND_TURN:
            board.roundTurn(mpx, mpy, turningRadius);
            break;
        case TURN_TO_LEFT:
            board.turnElement(x, y, LEFT);
            break;
        case TURN_TO_RIGHT:
            board.turnElement(x, y, RIGHT);
            break;
        default:
            isChanged = false;
        }

        if (isChanged)
            update();
    }

    if (event->button() == Qt::RightButton) {
        switch (command) {
        case PLACE_LINE:
            break;
        case PLACE_POLYGON:
            if (isDrawingLayer)
                board.addPolygon();
            break;
        case PLACE_SEGMENT:
            if (isElementLayer)
                board.addTrack();
            break;
        default:
            command = SELECT;
            board.selectedElement = false;
            board.showMessage = false;
            board.pointNumber = 0;
            isChanged = false;
        }

        if (isChanged)
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

    painter.fillRect(91, 0, 1160, 820, QColor(255, 255, 255, 255));

    painter.setPen(Qt::black);
    painter.setFont(serifFont);

    if (showGrid) {
        for (int i = 0; i < 1150 / gridStep; i++)
            for (int j = 0; j < 790 / gridStep; j++)
                painter.drawPoint(gridX + gridStep * i, gridY + gridStep * j);
    }

    // Draw message
    if (board.showMessage) {
        painter.setFont(serifFont2);
        painter.drawText(90, 890, board.message);
    }

    painter.translate(dx, dy);

    board.draw(painter, fontSize, scale);
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
    board.draw(painter, fontSize, scale);
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

void PcbEditor::selectCheckBox(int number)
{
    bool state = checkBox[number]->isChecked();

    switch (number) {
    case FILL_PADS:
        board.fillPads = state;
        break;
    case SHOW_GRID:
        showGrid = state;
        break;
    case SHOW_NETS:
        board.showNets = state;
        break;
    }

    update();
}

void PcbEditor::selectLayerCheckBox()
{
    for (int i = 0; i < layersNumber; i++)
        if (layerCheckBox[i]->isChecked())
            board.layers.draw |= (1 << i);
        else
            board.layers.draw &= ~(1 << i);

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
    bool ok;
    int newGrid;
    int tmp;
    QString str;

    switch (number) {
    case DEC_FONT_SIZE:
        fontSize -= 2;
    case INC_FONT_SIZE:
        fontSize++;
        limit(fontSize, minFontSize, maxFontSize);
        fontSizeLineEdit->setText(str.setNum(fontSize));
        break;
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
    case TURNING_RADIUS:
        tmp = turningRadiusLineEdit->text().toInt(&ok);
        if (ok)
            turningRadius = tmp;
        limit(turningRadius, minTurningRadius, maxTurningRadius);
        turningRadiusLineEdit->setText(str.setNum(turningRadius));
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

    bool isElementLayer = board.layers.edit == FRONT_LAYER ||
                          board.layers.edit == BACK_LAYER;
    bool isDrawingLayer = isElementLayer || board.layers.edit == BORDER_LAYER;

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
        if (isDrawingLayer)
            board.points.clear();
        break;
    case PLACE_SEGMENT:
        if (isElementLayer)
            board.pointNumber = 0;
        break;
    case ROUTE_TRACKS:
        board.routeTracks();
        break;
    case SEGMENT_NETS:
        if (isElementLayer)
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
