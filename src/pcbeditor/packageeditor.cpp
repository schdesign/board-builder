// packageeditor.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "packageeditor.h"
#include "ui_packageeditor.h"
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
#include <QRadioButton>
#include <QSvgGenerator>
#include <QTextStream>
#include <QVBoxLayout>

PackageEditor::PackageEditor(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    connect(actionNewFile, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(actionOpenFile, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionSaveFile, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(actionCloseFile, SIGNAL(triggered()), this, SLOT(closeFile()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(hide()));
    connect(actionNewPackage, SIGNAL(triggered()), this, SLOT(newPackage()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    QComboBox *tmpPackageComboBox[packageComboBoxes] =
    {
        addPadOrientationComboBox, addPadTypeComboBox, addPadsOrientationComboBox,
        addPadsTypeComboBox, nameTextAlignHComboBox, nameTextAlignVComboBox,
        padType0ShapeComboBox, padType1ShapeComboBox, padType2ShapeComboBox,
        referenceTextAlignHComboBox, referenceTextAlignVComboBox,
        selectedPadOrientationComboBox, selectedPadTypeComboBox, typeComboBox
    };

    std::copy(tmpPackageComboBox, tmpPackageComboBox + packageComboBoxes, packageComboBox);

    for (int i = 0; i < packageComboBoxes; i++)
        connect(packageComboBox[i], QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
        [=] (const QString &text) { selectComboBox(i, text); });

    QLineEdit *tmpPackageLineEdit[packageLineEdits] =
    {
        addEllipseHLineEdit, addEllipseWLineEdit, addEllipseXLineEdit,
        addEllipseYLineEdit, addLineX1LineEdit, addLineX2LineEdit,
        addLineY1LineEdit, addLineY2LineEdit, addPadNumberLineEdit,
        addPadXLineEdit, addPadYLineEdit, addPadsDxLineEdit,
        addPadsDyLineEdit, addPadsFirstLineEdit, addPadsFirstXLineEdit,
        addPadsFirstYLineEdit, addPadsLastLineEdit, borderBottomLineEdit,
        borderLeftXLineEdit, borderRightXLineEdit, borderTopYLineEdit,
        ellipsesLineEdit, linesLineEdit, nameLineEdit,
        nameTextHeightLineEdit, nameTextDownXLineEdit, nameTextLeftXLineEdit,
        nameTextRightXLineEdit, nameTextUpXLineEdit, nameTextDownYLineEdit,
        nameTextLeftYLineEdit, nameTextRightYLineEdit,nameTextUpYLineEdit,
        padType0LineEdit1, padType0LineEdit2, padType0LineEdit3, padType0LineEdit4,
        padType1LineEdit1, padType1LineEdit2, padType1LineEdit3, padType1LineEdit4,
        padType2LineEdit1, padType2LineEdit2, padType2LineEdit3, padType2LineEdit4,
        padsLineEdit, referenceTextHeightLineEdit, referenceTextDownXLineEdit,
        referenceTextLeftXLineEdit, referenceTextRightXLineEdit, referenceTextUpXLineEdit,
        referenceTextDownYLineEdit, referenceTextLeftYLineEdit, referenceTextRightYLineEdit,
        referenceTextUpYLineEdit, selectedEllipseHLineEdit, selectedEllipseWLineEdit,
        selectedEllipseXLineEdit, selectedEllipseYLineEdit, selectedLineX1LineEdit,
        selectedLineX2LineEdit, selectedLineY1LineEdit, selectedLineY2LineEdit,
        selectedPadNumberLineEdit, selectedPadXLineEdit, selectedPadYLineEdit
    };

    std::copy(tmpPackageLineEdit, tmpPackageLineEdit + packageLineEdits, packageLineEdit);

/*
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
*/

    padType0Label3->hide();
    padType0Label4->hide();
    padType1Label3->hide();
    padType1Label4->hide();
    padType2Label3->hide();
    padType2Label4->hide();

    padType0LineEdit3->hide();
    padType0LineEdit4->hide();
    padType1LineEdit3->hide();
    padType1LineEdit4->hide();
    padType2LineEdit3->hide();
    padType2LineEdit4->hide();

    tmpPackage.pads.resize(2);

    //QString str;
    //command = SELECT;
    //previousCommand = command;
    //mousePoint = QPoint(0, 0);
    //step = gridStep;
    //gridNumber = 6; // 1000 um in grid step
    //scale = double(gridStep) / grid[gridNumber];
    //gridLineEdit->setText(str.setNum(grid[gridNumber]));
    //space = package.defaultPolygonSpace;
    //spaceLineEdit->setText(str.setNum(space));
    //width = package.defaultLineWidth;
    //widthLineEdit->setText(str.setNum(width));
    //dx = gridX;
    //dy = gridY;
    //centerX = 57 * grid[gridNumber];
    //centerY = 40 * grid[gridNumber];
    //maxXLineEdit->setText(str.setNum(maxX));
    //maxYLineEdit->setText(str.setNum(maxY));
    //dxLineEdit->setText(str.setNum(dx));
    //dyLineEdit->setText(str.setNum(dy));
    //stepLineEdit->setText(str.setNum(step));
}

void PackageEditor::about()
{
    QMessageBox::information(this, tr("About Package Editor"),
        tr("Package Editor\n""Copyright (C) 2018 Alexander Karpeko"));
}

/*
void PackageEditor::buttonsSetEnabled(const char *params)
{
    for (int i = 0; i < maxButton; i++) {
        if (params[i] == '1')
            button[i]->setEnabled(true);
        if (params[i] == '0')
            button[i]->setEnabled(false);
    }
}
*/

void PackageEditor::closeFile()
{/*
    //package.clear();

    actionOpenFile->setEnabled(true);
    actionCloseFile->setEnabled(false);

    update();

    // Left, right, up, down, zoom in, zoom out
    // buttonsSetEnabled("000000");*/
}
/*
void PackageEditor::keyPressEvent(QKeyEvent *event)
{
    switch (command) {
    case PLACE_NET_NAME:
        if (package.selectedWire)
            if (event->text() != QString("q"))
                package.value += event->text();
            else
                package.addNetName(0, 0);
        break;
    case SET_VALUE:
        if (package.selectedElement)
            if (event->text() != QString("q"))
                package.value += event->text();
            else
                package.setValue(0, 0);
        break;
    }

    update();
}
*/
void PackageEditor::limit(int &value, int min, int max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
}
/*
void PackageEditor::mousePressEvent(QMouseEvent *event)
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
        case DELETE_LINE:
            package.deleteLine(x, y);
            break;
        case METER:
            meterLineEdit->setText(str.setNum(package.meter(x, y), 'f', 2));
            break;
        case MOVE:
            package.moveElement(x, y);
            break;
        case MOVE_GROUP:
            package.moveGroup(x, y, scale);
            break;
        case PLACE_LINE:
            package.addLinePoint(x, y, width);
            break;
       // case SET_VALUE:
       //     package.setValue(x, y);
       //     break;
        case TURN_TO_LEFT:
            package.turnElement(x, y, LEFT);
            break;
        case TURN_TO_RIGHT:
            package.turnElement(x, y, RIGHT);
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
            package.addTrack();
            break;
        default:
            command = SELECT;
            package.selectedElement = false;
            package.showMessage = false;
            package.pointNumber = 0;
        }

        update();
    }
}
*/
void PackageEditor::newFile()
{

}

void PackageEditor::newPackage()
{
    package.clear();
    showPackageData();
    update();
}

void PackageEditor::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open pkg file"),
                       packageDirectory, tr("pkg files (*.pkg)"));
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
        //package.fromJson(array);
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(this, tr("Error"), e.show());
        return;
    }

    actionOpenFile->setEnabled(false);
    actionCloseFile->setEnabled(true);

    update();
}

/*
void Board::readJsonFile(const QString &filename, QByteArray &byteArray)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        throw ExceptionData(filename + " open error");
    byteArray = file.readAll();
    file.close();
}

void Board::readPackageLibrary(const QString &libraryname)
{
    QByteArray byteArray;
    readJsonFile(libraryname, byteArray);

    QJsonDocument document(QJsonDocument::fromJson(byteArray));
    if (document.isNull())
        throw ExceptionData("Package library file read error");

    QJsonObject object = document.object();

    if (object["object"].toString() != "packageLibrary")
        throw ExceptionData("File is not a package library file");

    QJsonArray filenames(object["filenames"].toArray());

    for (auto f : filenames) {
        readJsonFile(packagesDirectory + "/" + f.toString(), byteArray);
        readPackages(byteArray);
    }
}

void Board::readPackages(const QByteArray &byteArray)
{
    QJsonParseError error;
    QString str;

    QJsonDocument document(QJsonDocument::fromJson(byteArray, &error));
    if (document.isNull())
        throw ExceptionData("Packages file read error: " + error.errorString() +
                            ", offset: " + str.setNum(error.offset));

    QJsonObject object = document.object();

    if (object["object"].toString() != "packages")
        throw ExceptionData("File is not a packages file");

    QJsonArray packageArray(object["packages"].toArray());
    for (auto p : packageArray)
        Element::addPackage(p);
}

void Element::addPackage(const QJsonValue &value)
{
    Package package;

    package.refX = 0;
    package.refY = 0;

    QJsonObject object = value.toObject();

    package.name = object["name"].toString();
    package.type = object["type"].toString();

    QJsonObject padParamsObject = object["padParams"].toObject();
    QJsonArray padsArray = object["pads"].toArray();
    QJsonArray linesArray = object["lines"].toArray();
    QJsonObject referenceTextObject = object["referenceText"].toObject();
    QJsonObject nameTextObject = object["nameText"].toObject();

    int width = padParamsObject["width"].toInt();
    int height = padParamsObject["height"].toInt();
    int radius = padParamsObject["radius"].toInt();

    for (int i = 0; i < padsArray.size(); i++) {
        QJsonObject packagePad = padsArray[i].toObject();
        Pad pad;
        pad.number = i + 1;
        pad.net = 0;
        pad.width = width;
        pad.height = height;
        pad.radius = radius;
        pad.orientation = packagePad["orientation"].toInt();
        pad.x = packagePad["x"].toInt();
        pad.y = packagePad["y"].toInt();
        package.pads.push_back(pad);
    }

    package.ellipse.fromJson(object["ellipse"]);

    for (auto l : linesArray) {
        Line line(l);
        package.lines.push_back(line);
    }

    package.border.fromJson(object["border"]);

    package.referenceTextHeight = referenceTextObject["height"].toInt();
    //package.referenceTextAlignmentX = referenceTextObject["alignmentX"].toInt();
    //package.referenceTextAlignmentY = referenceTextObject["alignmentY"].toInt();
    package.referenceTextX[0] = referenceTextObject["upX"].toInt();
    package.referenceTextY[0] = referenceTextObject["upY"].toInt();
    package.referenceTextX[1] = referenceTextObject["rightX"].toInt();
    package.referenceTextY[1] = referenceTextObject["rightY"].toInt();
    package.referenceTextX[2] = referenceTextObject["downX"].toInt();
    package.referenceTextY[2] = referenceTextObject["downY"].toInt();
    package.referenceTextX[3] = referenceTextObject["leftX"].toInt();
    package.referenceTextY[3] = referenceTextObject["leftY"].toInt();

    package.nameTextHeight = nameTextObject["height"].toInt();
    //package.nameTextAlignmentX = nameTextObject["alignmentX"].toInt();
    //package.nameTextAlignmentY = nameTextObject["alignmentY"].toInt();
    package.nameTextX[0] = nameTextObject["upX"].toInt();
    package.nameTextY[0] = nameTextObject["upY"].toInt();
    package.nameTextX[1] = nameTextObject["rightX"].toInt();
    package.nameTextY[1] = nameTextObject["rightY"].toInt();
    package.nameTextX[2] = nameTextObject["downX"].toInt();
    package.nameTextY[2] = nameTextObject["downY"].toInt();
    package.nameTextX[3] = nameTextObject["leftX"].toInt();
    package.nameTextY[3] = nameTextObject["leftY"].toInt();

    Element::packages.push_back(package);
}


*/

void PackageEditor::paintEvent(QPaintEvent *)
{
    constexpr int w = 680;
    constexpr int h = 600;

    QPainter painter(this);
    QFont serifFont("Times", 10, QFont::Normal);
    QFont serifFont2("Times", 12, QFont::Normal);

    painter.fillRect(gridX - gridStep, gridY - gridStep,
                     w + gridStep, h + gridStep, QColor(255, 255, 255, 255));

    painter.setPen(Qt::black);
    painter.setFont(serifFont);

    //if (showGridCheckBox->isChecked()) {
        for (int i = 0; i < w / gridStep; i++)
            for (int j = 0; j < h / gridStep; j++)
                painter.drawPoint(gridX + gridStep * i, gridY + gridStep * j);
    //}
/*
    // Draw message
    if (package.showMessage) {
        painter.setFont(serifFont2);
        painter.drawText(90, 890, package.message);
    }

    painter.translate(dx, dy);

    package.draw(painter, scale); */
}

void PackageEditor::saveFile()
{/*
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save pkg file"),
                          packageDirectory, tr("pkg files (*.pkg)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    //QJsonDocument document(package.toJson());
    //QByteArray array(document.toJson());

    //file.write(array);
    file.close();*/
}

void PackageEditor::selectComboBox(int number, const QString &text)
{
    constexpr int padTypeShapeComboBoxNumbers[3] =
    {
        PAD_TYPE_0_SHAPE, PAD_TYPE_1_SHAPE, PAD_TYPE_2_SHAPE
    };

    QComboBox *padTypeShapeComboBox[3] =
    {
        padType0ShapeComboBox, padType1ShapeComboBox, padType2ShapeComboBox
    };

    switch (number) {
    case ADD_PAD_ORIENTATION:
        if (text == "Up")
            tmpPackage.pads[1].orientation = 0;
        if (text == "Right")
            tmpPackage.pads[1].orientation = 1;
        break;
    case ADD_PAD_TYPE:
        if (text == "0" || text == "1" || text == "2")
            tmpPackage.pads[1].typeNumber = text.toInt();
        break;
    case ADD_PADS_ORIENTATION:
        if (text == "Up")
            tmpPackage.pads[2].orientation = 0;
        if (text == "Right")
            tmpPackage.pads[2].orientation = 1;
        break;
    case ADD_PADS_TYPE:
        if (text == "0" || text == "1" || text == "2")
            tmpPackage.pads[2].typeNumber = text.toInt();
        break;
    case NAME_TEXT_ALIGN_H:
        if (text == "Center" || text == "Left" || text == "Right")
            tmpPackage.nameTextAlignH = text;
        break;
    case NAME_TEXT_ALIGN_V:
        if (text == "Center" || text == "Top" || text == "Bottom")
            tmpPackage.nameTextAlignV = text;
        break;
    case PAD_TYPE_0_SHAPE:
    case PAD_TYPE_1_SHAPE:
    case PAD_TYPE_2_SHAPE:
        selectPadTypeComboBox(number, text);
        break;
    case REFERENCE_TEXT_ALIGN_H:
        if (text == "Center" || text == "Left" || text == "Right")
            tmpPackage.referenceTextAlignH = text;
        break;
    case REFERENCE_TEXT_ALIGN_V:
        if (text == "Center" || text == "Top" || text == "Bottom")
            tmpPackage.referenceTextAlignV = text;
        break;
    case SELECTED_PAD_ORIENTATION:
        if (text == "Up")
            tmpPackage.pads[0].orientation = 0;
        if (text == "Right")
            tmpPackage.pads[0].orientation = 1;
        break;
    case SELECTED_PAD_TYPE:
        if (text == "0" || text == "1" || text == "2")
            tmpPackage.pads[0].typeNumber = text.toInt();
        break;
    case PACKAGE_TYPE:
        if (text == "SMD" || text == "DIP") {
            tmpPackage.type = text;
            for (int i = 0; i < 3; i++) {
                QString text2 = padTypeShapeComboBox[i]->currentText();
                selectPadTypeComboBox(padTypeShapeComboBoxNumbers[i], text2);
            }
        }
        break;
    }
}

void PackageEditor::selectPadTypeComboBox(int number, const QString &text)
{
    QLabel *padTypeLabel[3][4] =
    {
        { padType0Label1, padType0Label2, padType0Label3, padType0Label4 },
        { padType1Label1, padType1Label2, padType1Label3, padType1Label4 },
        { padType2Label1, padType2Label2, padType2Label3, padType2Label4 }
    };

    QLineEdit *padTypeLineEdit[3][4] =
    {
        { padType0LineEdit1, padType0LineEdit2, padType0LineEdit3, padType0LineEdit4 },
        { padType1LineEdit1, padType1LineEdit2, padType1LineEdit3, padType1LineEdit4 },
        { padType2LineEdit1, padType2LineEdit2, padType2LineEdit3, padType2LineEdit4 }
    };

    int padType = -1;

    switch (number) {
    case PAD_TYPE_0_SHAPE:
        padType = 0;
    case PAD_TYPE_1_SHAPE:
        if (padType == -1)
            padType = 1;
    case PAD_TYPE_2_SHAPE:
        if (padType == -1)
            padType = 2;
        if (text == "Rectangle") {
            padTypeLabel[padType][0]->setText("w");
            padTypeLabel[padType][0]->show();
            padTypeLineEdit[padType][0]->show();
            padTypeLabel[padType][1]->setText("h");
            padTypeLabel[padType][1]->show();
            padTypeLineEdit[padType][1]->show();
            if (tmpPackage.type == "DIP") {
                padTypeLabel[padType][2]->setText("d in");
                padTypeLabel[padType][2]->show();
                padTypeLineEdit[padType][2]->show();
            }
            else {
                padTypeLabel[padType][2]->hide();
                padTypeLineEdit[padType][2]->hide();
            }
            padTypeLabel[padType][3]->hide();
            padTypeLineEdit[padType][3]->hide();
        }
        if (text == "Rounded rectangle") {
            padTypeLabel[padType][0]->setText("w");
            padTypeLabel[padType][0]->show();
            padTypeLineEdit[padType][0]->show();
            padTypeLabel[padType][1]->setText("h");
            padTypeLabel[padType][1]->show();
            padTypeLineEdit[padType][1]->show();
            padTypeLabel[padType][2]->setText("r");
            padTypeLabel[padType][2]->show();
            padTypeLineEdit[padType][2]->show();
            if (tmpPackage.type == "DIP") {
                padTypeLabel[padType][3]->setText("d in");
                padTypeLabel[padType][3]->show();
                padTypeLineEdit[padType][3]->show();
            }
            else {
                padTypeLabel[padType][3]->hide();
                padTypeLineEdit[padType][3]->hide();
            }
        }
        if (text == "Circle") {
            padTypeLabel[padType][0]->setText("d");
            padTypeLabel[padType][0]->show();
            padTypeLineEdit[padType][0]->show();
            if (tmpPackage.type == "DIP") {
                padTypeLabel[padType][1]->setText("d in");
                padTypeLabel[padType][1]->show();
                padTypeLineEdit[padType][1]->show();
            }
            else {
                padTypeLabel[padType][1]->hide();
                padTypeLineEdit[padType][1]->hide();
            }
            padTypeLabel[padType][2]->hide();
            padTypeLineEdit[padType][2]->hide();
            padTypeLabel[padType][3]->hide();
            padTypeLineEdit[padType][3]->hide();
        }
        break;
    }
}

/*
void PackageEditor::selectCheckBox()
{
    for (int i = 0; i < checkBoxes; i++)
        if (checkBox[i]->isChecked())
            package.layers.draw |= (1 << i);
        else
            package.layers.draw &= ~(1 << i);

    if (showNetsCheckBox->isChecked())
        package.showNets = true;
    else
        package.showNets = false;

    update();
}
*/

/*
void PackageEditor::selectPushButton(int number)
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
*/

/*
void PackageEditor::selectRadioButton()
{
    for (int i = 0; i < radioButtons; i++)
        if (radioButton[i]->isChecked())
            package.layers.edit = i - 1;
}
*/

/*
void PackageEditor::selectToolButton(int number)
{
    int tmp;
    QString str;

    previousCommand = command;
    command = number;

    switch (command) {
    case CREATE_GROUPS:
        package.createGroups();
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
        package.selectedElement = false;
        break;
    case MOVE_DOWN:
        dy += 2 * step;
    case MOVE_UP:
        dy -= step;
        centerY = (gridY + 400 - dy) / scale + 0.5;
        dyLineEdit->setText(str.setNum(dy));
        break;
    case MOVE_GROUP:
        package.points.clear();
        break;
    case MOVE_LEFT:
        dx -= 2 * step;
    case MOVE_RIGHT:
        dx += step;
        centerX = (gridX + 570 - dx) / scale + 0.5;
        dxLineEdit->setText(str.setNum(dx));
        break;        
    case PLACE_DEVICE:
        selectDevice(package.deviceNameID);
        if (package.deviceNameID == -1)
            command = SELECT;
        break;
    case PLACE_NET_NAME:
        package.selectedWire = false;
        break;
    case PLACE_ELEMENTS:
        package.placeElements();
        break;
    case PLACE_LINE:
        break;
    case PLACE_POLYGON:
        if (package.layers.edit == FRONT || package.layers.edit == BACK ||
            package.layers.edit == BORDER)
            package.points.clear();
        break;
    case PLACE_SEGMENT:
        if (package.layers.edit == FRONT || package.layers.edit == BACK)
            package.pointNumber = 0;
        break;
    case ROUTE_TRACKS:
        package.routeTracks();
        break;
    case SEGMENT_NETS:
        if (package.layers.edit == FRONT || package.layers.edit == BACK)
            package.segmentNets();
        break;
    case SELECT:
        package.selectedElement = false;
        package.showMessage = false;
        package.pointNumber = 0;
        break;
    case SET_VALUE:
        package.selectedElement = false;
        break;
    case SHOW_GROUND_NETS:
        package.showGroundNets = true;
        std::cerr << "package.showGroundNets = true" << std::endl;
        package.showGroundNets ^= 1;
        break;
    case UPDATE_NETS:
        break;
    case TABLE_ROUTE:
        package.tableRoute();
        break;
    case WAVE_ROUTE:
        stepLineEdit->setText(str.setNum(0));
        tmp = package.waveRoute();
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
*/

void PackageEditor::showPackageData()
{/*
    QString str;
    package.border;
    package.ellipse;
    package.nameTextHeight;
    package.nameTextAlignmentX;
    package.nameTextAlignmentY;
    package.nameTextX[4];
    package.nameTextY[4];
    package.referenceTextHeight;
    package.referenceTextAlignmentX;
    package.referenceTextAlignmentY;
    package.referenceTextX[4];
    package.referenceTextY[4];
    package.refX;
    package.refY;
    package.name;
    package.type;
    package.lines;
    package.pads; */
}

/*
    {
        addPadOrientationComboBox, addPadTypeComboBox, addPadsOrientationComboBox,
        addPadsTypeComboBox, nameTextAlignHComboBox, nameTextAlignVComboBox,
        padType0ShapeComboBox, padType1ShapeComboBox, padType2ShapeComboBox,
        referenceTextAlignHComboBox, referenceTextAlignVComboBox,typeComboBox
        selectedPadOrientationComboBox, selectedPadTypeComboBox, typeComboBox
    };

    {
        addEllipseHLineEdit, addEllipseWLineEdit, addEllipseXLineEdit,
        addEllipseYLineEdit, addLineX1LineEdit, addLineX2LineEdit,
        addLineY1LineEdit, addLineY2LineEdit, addPadNumberLineEdit,
        addPadXLineEdit, addPadYLineEdit, addPadsDxLineEdit,
        addPadsDyLineEdit, addPadsFirstLineEdit, addPadsFirstXLineEdit,
        addPadsFirstYLineEdit, addPadsLastLineEdit, borderBottomLineEdit,
        borderLeftXLineEdit, borderRightXLineEdit, borderTopYLineEdit,
        ellipsesLineEdit, linesLineEdit, nameLineEdit,
        nameTextHeightLineEdit, nameTextDownXLineEdit, nameTextLeftXLineEdit,
        nameTextRightXLineEdit, nameTextUpXLineEdit, nameTextDownYLineEdit,
        nameTextLeftYLineEdit, nameTextRightYLineEdit,nameTextUpYLineEdit,
        padType0LineEdit1, padType0LineEdit2, padType0LineEdit3,
        padType1LineEdit1, padType1LineEdit2, padType1LineEdit3,
        padType2LineEdit1, padType2LineEdit2, padType2LineEdit3,
        padsLineEdit, referenceTextHeightLineEdit, referenceTextDownXLineEdit,
        referenceTextLeftXLineEdit, referenceTextRightXLineEdit, referenceTextUpXLineEdit,
        referenceTextDownYLineEdit, referenceTextLeftYLineEdit, referenceTextRightYLineEdit,
        referenceTextUpYLineEdit, selectedEllipseHLineEdit, selectedEllipseWLineEdit,
        selectedEllipseXLineEdit, selectedEllipseYLineEdit, selectedLineX1LineEdit,
        selectedLineX2LineEdit, selectedLineY1LineEdit, selectedLineY2LineEdit,
        selectedPadNumberLineEdit, selectedPadXLineEdit, selectedPadYLineEdit
    };
*/

void PackageEditor::updatePackage()
{
/*
    border.bottomY = borderBottomLineEdit->text().toInt();
    border.leftX = borderLeftXLineEdit->text().toInt();
    border.rightX = borderRightXLineEdit->text().toInt();
    border.topY = borderTopYLineEdit->text().toInt();

    ellipse.h = selectedEllipseHLineEdit->text().toInt();
    ellipse.w = selectedEllipseWLineEdit->text().toInt();
    ellipse.x = selectedEllipseXLineEdit->text().toInt();
    ellipse.y = selectedEllipseYLineEdit->text().toInt();

    nameTextHeight = nameTextHeightLineEdit->text().toInt();
    nameTextAlignH = nameTextAlignHComboBox->currentText().toInt();
    nameTextAlignV = nameTextAlignVComboBox->currentText().toInt();
    nameTextX[0] = nameTextUpXLineEdit->text().toInt();
    nameTextY[0] = nameTextUpYLineEdit->text().toInt();
    nameTextX[1] = nameTextRightXLineEdit->text().toInt();
    nameTextY[1] = nameTextRightYLineEdit->text().toInt();
    nameTextX[2] = nameTextDownXLineEdit->text().toInt();
    nameTextY[2] = nameTextDownYLineEdit->text().toInt();
    nameTextX[3] = nameTextLeftXLineEdit->text().toInt();
    nameTextY[3] = nameTextLeftYLineEdit->text().toInt();

    referenceTextHeight = referenceTextHeightLineEdit->text().toInt();
    referenceTextAlignH = referenceTextAlignHComboBox->currentText().toInt();
    referenceTextAlignV = referenceTextAlignVComboBox->currentText().toInt();
    referenceTextX[0] = referenceTextUpXLineEdit->text().toInt();
    referenceTextY[0] = referenceTextUpYLineEdit->text().toInt();
    referenceTextX[1] = referenceTextRightXLineEdit->text().toInt();
    referenceTextY[1] = referenceTextRightYLineEdit->text().toInt();
    referenceTextX[2] = referenceTextDownXLineEdit->text().toInt();
    referenceTextY[2] = referenceTextDownYLineEdit->text().toInt();
    referenceTextX[3] = referenceTextLeftXLineEdit->text().toInt();
    referenceTextY[3] = referenceTextLeftYLineEdit->text().toInt();

    name = nameLineEdit->text();
    type = typeComboBox->currentText();
    ellipsesLineEdit->setText(QString::number(ellipses.size()));
    linesLineEdit->setText(QString::number(lines.size()));
    padsLineEdit->setText(QString::number(pads.size()));
*/
}
