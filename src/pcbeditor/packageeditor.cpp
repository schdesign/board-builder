// packageeditor.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "exceptiondata.h"
#include "function.h"
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

    QCheckBox *tmpCheckBox[checkBoxes] =
    {
        borderCheckBox, packageCheckBox, padsCheckBox
    };

    std::copy(tmpCheckBox, tmpCheckBox + checkBoxes, checkBox);

    for (int i = 0; i < checkBoxes; i++)
        connect(checkBox[i], &QCheckBox::clicked, [=] () { selectCheckBox(i); });

    QComboBox *tmpComboBox[comboBoxes] =
    {
        addPadOrientationComboBox, addPadTypeComboBox, addPadsOrientationComboBox,
        addPadsTypeComboBox, padType0ShapeComboBox, padType1ShapeComboBox,
        padType2ShapeComboBox, selectedPadOrientationComboBox,
        selectedPadTypeComboBox, typeComboBox
    };

    std::copy(tmpComboBox, tmpComboBox + comboBoxes, comboBox);

    for (int i = 0; i < comboBoxes; i++)
        connect(comboBox[i], QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
        [=] (const QString &text) { selectComboBox(i, text); });

    QLineEdit *tmpLineEdit[lineEdits] =
    {
        addEllipseHLineEdit, addEllipseWLineEdit, addEllipseXLineEdit, addEllipseYLineEdit,
        addLineX1LineEdit, addLineX2LineEdit, addLineY1LineEdit, addLineY2LineEdit,
        addPadNumberLineEdit, addPadXLineEdit, addPadYLineEdit, addPadsDxLineEdit,
        addPadsDyLineEdit, addPadsFirstLineEdit, addPadsFirstXLineEdit, addPadsFirstYLineEdit,
        addPadsLastLineEdit, borderBottomLineEdit, borderLeftXLineEdit, borderRightXLineEdit,
        borderTopYLineEdit, deleteEllipsesFirstLineEdit, deleteEllipsesLastLineEdit,
        deleteLinesFirstLineEdit, deleteLinesLastLineEdit, deletePadsFirstLineEdit,
        deletePadsLastLineEdit, ellipsesLineEdit, gridLineEdit, linesLineEdit, nameLineEdit,
        padType0LineEdit1, padType0LineEdit2, padType0LineEdit3, padType0LineEdit4,
        padType1LineEdit1, padType1LineEdit2, padType1LineEdit3, padType1LineEdit4,
        padType2LineEdit1, padType2LineEdit2, padType2LineEdit3, padType2LineEdit4,
        padsLineEdit, selectedEllipseHLineEdit, selectedEllipseWLineEdit,
        selectedEllipseXLineEdit, selectedEllipseYLineEdit, selectedLineX1LineEdit,
        selectedLineX2LineEdit, selectedLineY1LineEdit, selectedLineY2LineEdit,
        selectedPadNumberLineEdit, selectedPadXLineEdit, selectedPadYLineEdit
    };

    std::copy(tmpLineEdit, tmpLineEdit + lineEdits, lineEdit);

    QRadioButton *tmpRadioButton[radioButtons] =
    {
        addEllipseRadioButton, addLineRadioButton, addPadRadioButton, addPadsRadioButton,
        borderRadioButton, deleteEllipsesRadioButton, deleteLinesRadioButton,
        deletePadsRadioButton, nameRadioButton, padTypesRadioButton, readOnlyModeRadioButton,
        selectedEllipseRadioButton, selectedLineRadioButton, selectedPadRadioButton,
        typeRadioButton
    };

    std::copy(tmpRadioButton, tmpRadioButton + radioButtons, radioButton);

    for (int i = 0; i < radioButtons; i++)
        connect(radioButton[i], &QRadioButton::clicked, [=] () { selectRadioButton(i); });

    QPushButton *tmpPushButton[pushButtons] =
    {
        cancelPushButton, centerPushButton, decGridPushButton,
        incGridPushButton, updatePushButton
    };

    std::copy(tmpPushButton, tmpPushButton + pushButtons, pushButton);

    for (int i = 0; i < pushButtons; i++)
        connect(pushButton[i], &QPushButton::clicked, [=] () { selectPushButton(i); });

/*
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

    QLineEdit *tmpPadTypeLineEdit[maxPadTypes][maxPadParams] =
    {
        { padType0LineEdit1, padType0LineEdit2, padType0LineEdit3, padType0LineEdit4 },
        { padType1LineEdit1, padType1LineEdit2, padType1LineEdit3, padType1LineEdit4 },
        { padType2LineEdit1, padType2LineEdit2, padType2LineEdit3, padType2LineEdit4 }
    };

    for (int i = 0; i < maxPadTypes; i++)
        for (int j = 0; j < maxPadParams; j++)
            padTypeLineEdit[i][j] = tmpPadTypeLineEdit[i][j];

    QComboBox *tmpPadTypeShapeComboBox[maxPadTypes] =
    {
        padType0ShapeComboBox, padType1ShapeComboBox, padType2ShapeComboBox
    };

    std::copy(tmpPadTypeShapeComboBox, tmpPadTypeShapeComboBox + maxPadTypes,
              padTypeShapeComboBox);

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

    package.clear();
    tmpPackage.clear();

    package.type = "SMD";

    QString str;

    //command = SELECT;
    //previousCommand = command;
    //mousePoint = QPoint(0, 0);
    //step = gridStep;

    gridNumber = 6;  // 250 um in grid step
    scale = double(gridStep) / grid[gridNumber];
    gridLineEdit->setText(str.setNum(grid[gridNumber]));

    //space = package.defaultPolygonSpace;
    //spaceLineEdit->setText(str.setNum(space));
    //width = package.defaultLineWidth;
    //widthLineEdit->setText(str.setNum(width));

    dx = gridX;
    dy = gridY;

    centerX = grid[gridNumber] * ((gridWidth / gridStep - 1) / 2);
    centerY = grid[gridNumber] * ((gridHeight / gridStep - 1) / 2);

    //maxXLineEdit->setText(str.setNum(maxX));
    //maxYLineEdit->setText(str.setNum(maxY));
    //dxLineEdit->setText(str.setNum(dx));
    //dyLineEdit->setText(str.setNum(dy));
    //stepLineEdit->setText(str.setNum(step));

    refX = centerX;
    refY = centerY;
    orientation = 0;  // "Up"

    updateElement();
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

void PackageEditor::centerElement()
{
    Border border = element.outerBorder;

    if (border.leftX >= border.rightX ||
        border.topY >= border.bottomY)
        return;

    int cx = (border.leftX + border.rightX) / 2;
    int cy = (border.topY + border.bottomY) / 2;

    refX += centerX - cx;
    refY += centerY - cy;

    int step = grid[gridNumber];
    refX = step * (refX / step);
    refY = step * (refY / step);
}

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
        readPackages(array);
        if (!packages.empty()) {
            package = packages[0];
            updateElement();
            centerElement();
            updateElement();
        }
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(this, tr("Error"), e.show());
        return;
    }

    actionOpenFile->setEnabled(false);
    actionCloseFile->setEnabled(true);

    update();
}

QString PackageEditor::padShape(const PadTypeParams &padTypeParams)
{
    bool isDiameter = padTypeParams.diameter > 0;
    bool isRectangle = padTypeParams.height > 0 && padTypeParams.width > 0;
    QString text;

    if (!isDiameter && isRectangle)
        text = "Rectangle";
    if (isDiameter && isRectangle)
        text = "Rounded rectangle";
    if (isDiameter && !isRectangle)
        text = "Circle";

    return text;
}

void PackageEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont serifFont("Times", 10, QFont::Normal);
    QFont serifFont2("Times", 12, QFont::Normal);

    painter.fillRect(gridX - gridStep, gridY - gridStep,
                     gridWidth + gridStep, gridHeight + gridStep,
                     QColor(255, 255, 255, 255));

    painter.setPen(Qt::black);
    painter.setFont(serifFont);

    //if (showGridCheckBox->isChecked()) {
        for (int i = 0; i < gridWidth / gridStep; i++)
            for (int j = 0; j < gridHeight / gridStep; j++)
                painter.drawPoint(gridX + gridStep * i, gridY + gridStep * j);
    //}

    painter.translate(dx, dy);

    if (layers.draw & (1 << BORDER_LAYER)) {
        painter.setPen(layers.color[BORDER_LAYER]);
        int x = scale * element.border.leftX;
        int y = scale * element.border.topY;
        int w = scale * (element.border.rightX - element.border.leftX);
        int h = scale * (element.border.bottomY - element.border.topY);
        painter.drawRect(x, y, w, h);
    }

    ElementDrawingOptions options;
    options.fillPads = true;
    options.scale = scale;
    options.fontSize = 0;
    element.draw(painter, layers, options);
}

void PackageEditor::readPackages(const QByteArray &byteArray)
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
    for (auto p : packageArray) {
        Package package(p);
        packages.push_back(package);
    }
}

void PackageEditor::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save pkg file"),
                           packageDirectory, tr("pkg files (*.pkg)"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;

    QJsonArray packagesArray;

    packagesArray.append(package.toJson());

    QJsonObject packages
    {
        {"object", "packages"},
        {"packages", packagesArray}
    };

    QJsonDocument document(packages);
    QByteArray array(document.toJson());

    file.write(array);
    file.close();
}

void PackageEditor::selectCheckBox(int number)
{
    bool state = checkBox[number]->isChecked();

    switch (number) {
    case SHOW_BORDER:
        setRadioButton(borderRadioButton, state);
        showLayer(BORDER_LAYER, state);
        break;
    case SHOW_PACKAGE:
        setRadioButton(addEllipseRadioButton, state);
        setRadioButton(addLineRadioButton, state);
        setRadioButton(deleteEllipsesRadioButton, state);
        setRadioButton(deleteLinesRadioButton, state);
        setRadioButton(selectedEllipseRadioButton, state);
        setRadioButton(selectedLineRadioButton, state);
        showLayer(PACKAGE_LAYER, state);
        break;
    case SHOW_PADS:
        setRadioButton(addPadRadioButton, state);
        setRadioButton(addPadsRadioButton, state);
        setRadioButton(deletePadsRadioButton, state);
        setRadioButton(padTypesRadioButton, state);
        setRadioButton(selectedPadRadioButton, state);
        showLayer(PAD_LAYER, state);
        break;
    }

    update();
}

void PackageEditor::showLayer(int number, bool state)
{
    if (state)
        layers.draw |= (1 << number);
    else
        layers.draw &= ~(1 << number);
}

void PackageEditor::selectComboBox(int number, const QString &text)
{
    constexpr int padTypeShapeComboBoxNumbers[maxPadTypes] =
    {
        PAD_TYPE_0_SHAPE, PAD_TYPE_1_SHAPE, PAD_TYPE_2_SHAPE
    };

    switch (number) {
    case PAD_TYPE_0_SHAPE:
    case PAD_TYPE_1_SHAPE:
    case PAD_TYPE_2_SHAPE:
        selectPadTypeComboBox(number, text);
        break;
    case PACKAGE_TYPE:
        if (text == "SMD" || text == "DIP") {
            tmpPackage.type = text;
            for (int i = 0; i < maxPadTypes; i++) {
                QString text2 = padTypeShapeComboBox[i]->currentText();
                selectPadTypeComboBox(padTypeShapeComboBoxNumbers[i], text2);
                for (int j = 0; j < maxPadParams; j++)
                    padTypeLineEdit[i][j]->setReadOnly(true);
            }
        }
        break;
    }
}

void PackageEditor::selectPadTypeComboBox(int number, const QString &text)
{
    QLabel *padTypeLabel[maxPadTypes][maxPadParams] =
    {
        { padType0Label1, padType0Label2, padType0Label3, padType0Label4 },
        { padType1Label1, padType1Label2, padType1Label3, padType1Label4 },
        { padType2Label1, padType2Label2, padType2Label3, padType2Label4 }
    };

    bool isDip = (tmpPackage.type == "DIP") ||
                 (package.type == "DIP" && tmpPackage.type != "SMD");
    int t = -1;  // padType

    switch (number) {
    case PAD_TYPE_0_SHAPE:
        t = 0;
    case PAD_TYPE_1_SHAPE:
        if (t == -1)
            t = 1;
    case PAD_TYPE_2_SHAPE:
        if (t == -1)
            t = 2;
        padTypeShape[t] = text;
        if (text == "Rectangle") {
            setPadTypeLineEdit(padTypeLabel[t][0], "w", padTypeLineEdit[t][0], "", true);
            setPadTypeLineEdit(padTypeLabel[t][1], "h", padTypeLineEdit[t][1], "", true);
            if (isDip)
                setPadTypeLineEdit(padTypeLabel[t][2], "d in", padTypeLineEdit[t][2], "", true);
            else
                setPadTypeLineEdit(padTypeLabel[t][2], "", padTypeLineEdit[t][2], "", false);
            setPadTypeLineEdit(padTypeLabel[t][3], "", padTypeLineEdit[t][3], "", false);
        }
        if (text == "Rounded rectangle") {
            setPadTypeLineEdit(padTypeLabel[t][0], "w", padTypeLineEdit[t][0], "", true);
            setPadTypeLineEdit(padTypeLabel[t][1], "h", padTypeLineEdit[t][1], "", true);
            setPadTypeLineEdit(padTypeLabel[t][2], "r", padTypeLineEdit[t][2], "", true);
            if (isDip)
                setPadTypeLineEdit(padTypeLabel[t][3], "d in", padTypeLineEdit[t][3], "", true);
            else
                setPadTypeLineEdit(padTypeLabel[t][3], "", padTypeLineEdit[t][3], "", false);
        }
        if (text == "Circle") {
            setPadTypeLineEdit(padTypeLabel[t][0], "d",
                               padTypeLineEdit[t][0], "", true);
            if (isDip)
                setPadTypeLineEdit(padTypeLabel[t][1], "d in",
                                   padTypeLineEdit[t][1], "", true);
            else
                setPadTypeLineEdit(padTypeLabel[t][1], "", padTypeLineEdit[t][1], "", false);
            setPadTypeLineEdit(padTypeLabel[t][2], "", padTypeLineEdit[t][2], "", false);
            setPadTypeLineEdit(padTypeLabel[t][3], "", padTypeLineEdit[t][3], "", false);
        }
        break;
    }

    if (t >= 0)
        for (int i = 0; i < maxPadParams; i++)
            if (padTypeLineEdit[t][i]->isVisible())
                padTypeLineEdit[t][i]->setReadOnly(false);
}

void PackageEditor::selectPushButton(int number)
{
    int newGrid;
    QString str;

    switch (number) {
    case CANCEL:
        showPackageData();
        break;
    case CENTER:
        centerElement();
        updateElement();
        break;
    case DEC_GRID:
        gridNumber -= 2;
    case INC_GRID:
        gridNumber++;
        limit(gridNumber, 0, grids - 1);
        newGrid = grid[gridNumber];
        scale = double(gridStep) / newGrid;
        centerX = newGrid * ((gridWidth / gridStep - 1) / 2);
        centerY = newGrid * ((gridHeight / gridStep - 1) / 2);
        gridLineEdit->setText(str.setNum(newGrid));
        centerElement();
        updateElement();
        break;
    case UPDATE:
        updatePackage();
        updateElement();
        break;
    }

    update();
}

void PackageEditor::selectRadioButton(int number)
{
    static int previousNumber = READ_ONLY_MODE;

    if (number == previousNumber)
        return;

    tmpPackage.clear();
    showPackageData();

    selectRadioButton(previousNumber, false);
    selectRadioButton(number, true);

    bool state = false;
    if (number != READ_ONLY_MODE)
        state = true;
    cancelPushButton->setEnabled(state);
    updatePushButton->setEnabled(state);

    previousNumber = number;
}

void PackageEditor::selectRadioButton(int number, bool state)
{
    QString str;

    switch (number) {
    case ADD_ELLIPSE:
        addEllipseHLineEdit->setReadOnly(!state);
        addEllipseWLineEdit->setReadOnly(!state);
        addEllipseXLineEdit->setReadOnly(!state);
        addEllipseYLineEdit->setReadOnly(!state);
        break;
    case ADD_LINE:
        addLineX1LineEdit->setReadOnly(!state);
        addLineX2LineEdit->setReadOnly(!state);
        addLineY1LineEdit->setReadOnly(!state);
        addLineY2LineEdit->setReadOnly(!state);
        break;
    case ADD_PAD:
        addPadOrientationComboBox->setEnabled(state);
        addPadTypeComboBox->setEnabled(state);
        if (state && package.pads.empty()) {
            addPadXLineEdit->setReadOnly(true);
            addPadYLineEdit->setReadOnly(true);
            addPadXLineEdit->setText("0");
            addPadYLineEdit->setText("0");
        }
        else {
            addPadXLineEdit->setReadOnly(!state);
            addPadYLineEdit->setReadOnly(!state);
        }
        break;
    case ADD_PADS:
        addPadsDxLineEdit->setReadOnly(!state);
        addPadsDyLineEdit->setReadOnly(!state);
        if (state && package.pads.empty()) {
            addPadsFirstXLineEdit->setReadOnly(true);
            addPadsFirstYLineEdit->setReadOnly(true);
            addPadsFirstXLineEdit->setText("0");
            addPadsFirstYLineEdit->setText("0");
        }
        else {
            addPadsFirstXLineEdit->setReadOnly(!state);
            addPadsFirstYLineEdit->setReadOnly(!state);
        }
        addPadsLastLineEdit->setReadOnly(!state);
        addPadsOrientationComboBox->setEnabled(state);
        addPadsTypeComboBox->setEnabled(state);
        break;
    case BORDER:
        borderBottomLineEdit->setReadOnly(!state);
        borderLeftXLineEdit->setReadOnly(!state);
        borderRightXLineEdit->setReadOnly(!state);
        borderTopYLineEdit->setReadOnly(!state);
        break;
    case DELETE_ELLIPSES:
        deleteEllipsesFirstLineEdit->setReadOnly(!state);
        deleteEllipsesLastLineEdit->setText(str.setNum(package.ellipses.size()));
        break;
    case DELETE_LINES:
        deleteLinesFirstLineEdit->setReadOnly(!state);
        deleteLinesLastLineEdit->setText(str.setNum(package.lines.size()));
        break;
    case DELETE_PADS:
        deletePadsFirstLineEdit->setReadOnly(!state);
        deletePadsLastLineEdit->setText(str.setNum(package.pads.size()));
        break;
    case NAME:
        nameLineEdit->setReadOnly(!state);
        break;
    case PAD_TYPES:
        padType0ShapeComboBox->setEnabled(state);
        padType1ShapeComboBox->setEnabled(state);
        padType2ShapeComboBox->setEnabled(state);
        for (int i = 0; i < maxPadTypes; i++)
            for (int j = 0; j < maxPadParams; j++)
                if (padTypeLineEdit[i][j]->isVisible())
                    padTypeLineEdit[i][j]->setReadOnly(!state);
        break;
    case READ_ONLY_MODE:
        break;
    case SELECTED_ELLIPSE:
        selectedEllipseHLineEdit->setReadOnly(!state);
        selectedEllipseWLineEdit->setReadOnly(!state);
        selectedEllipseXLineEdit->setReadOnly(!state);
        selectedEllipseYLineEdit->setReadOnly(!state);
        break;
    case SELECTED_LINE:
        selectedLineX1LineEdit->setReadOnly(!state);
        selectedLineX2LineEdit->setReadOnly(!state);
        selectedLineY1LineEdit->setReadOnly(!state);
        selectedLineY2LineEdit->setReadOnly(!state);
        break;
    case SELECTED_PAD:
        selectedPadNumberLineEdit->setReadOnly(!state);
        selectedPadOrientationComboBox->setEnabled(state);
        selectedPadTypeComboBox->setEnabled(state);
        selectedPadXLineEdit->setReadOnly(!state);
        selectedPadYLineEdit->setReadOnly(!state);
        break;
    case TYPE:
        typeComboBox->setEnabled(state);
        break;
    }
}

void PackageEditor::setPadTypeLineEdit(QLabel *label, const QString &labelText,
                                       QLineEdit *lineEdit, const QString &lineEditText,
                                       bool state)
{
    if (state) {
        label->setText(labelText);
        lineEdit->setText(lineEditText);
        label->show();
        lineEdit->show();
    }
    else {
        label->hide();
        lineEdit->hide();
        label->clear();
        lineEdit->clear();
    }
}

void PackageEditor::setPadTypeTexts(int padTypeNumber, int n1, int n2, int n3, int n4)
{
    int n[maxPadParams] = {n1, n2, n3, n4};

    for (int i = 0; i < maxPadParams; i++)
        if (n[i] > 0)
            padTypeLineEdit[padTypeNumber][i]->setText(QString::number(n[i]));
        else
            padTypeLineEdit[padTypeNumber][i]->clear();
}

void PackageEditor::setRadioButton(QRadioButton *button, bool state)
{
    button->setEnabled(state);

    if (!state && button->isChecked())
        readOnlyModeRadioButton->click();
}

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
{
    QString str;

    nameLineEdit->setText(package.name);
    typeComboBox->setCurrentText(package.type);

    borderBottomLineEdit->setText(str.setNum(package.border.bottomY));
    borderLeftXLineEdit->setText(str.setNum(package.border.leftX));
    borderRightXLineEdit->setText(str.setNum(package.border.rightX));
    borderTopYLineEdit->setText(str.setNum(package.border.topY));

    selectedEllipseHLineEdit->clear();
    selectedEllipseWLineEdit->clear();
    selectedEllipseXLineEdit->clear();
    selectedEllipseYLineEdit->clear();

    addEllipseHLineEdit->clear();
    addEllipseWLineEdit->clear();
    addEllipseXLineEdit->clear();
    addEllipseYLineEdit->clear();

    deleteEllipsesFirstLineEdit->clear();
    deleteEllipsesLastLineEdit->clear();

    selectedLineX1LineEdit->clear();
    selectedLineX2LineEdit->clear();
    selectedLineY1LineEdit->clear();
    selectedLineY2LineEdit->clear();

    addLineX1LineEdit->clear();
    addLineX2LineEdit->clear();
    addLineY1LineEdit->clear();
    addLineY2LineEdit->clear();

    deleteLinesFirstLineEdit->clear();
    deleteLinesLastLineEdit->clear();

    for (int i = 0; i < maxPadTypes; i++) {
        if (i < package.padTypesParams.size()) {
            PadTypeParams &params = package.padTypesParams[i];
            int innerDiameter = 0;
            if (package.type == "DIP")
                innerDiameter = params.innerDiameter;
            QString text = padShape(params);
            if (text.isEmpty()) {
                for (int j = 0; j < maxPadParams; j++)
                    padTypeLineEdit[i][j]->clear();
                break;
            }
            selectPadTypeComboBox(i, text);
            if (text == "Rectangle")
                setPadTypeTexts(i, params.width, params.height, innerDiameter, 0);
            if (text == "Rounded rectangle")
                setPadTypeTexts(i, params.width, params.height,
                                params.diameter / 2, innerDiameter);
            if (text == "Circle")
                setPadTypeTexts(i, params.diameter, innerDiameter, 0, 0);
        }
        else
            for (int j = 0; j < maxPadParams; j++)
                padTypeLineEdit[i][j]->clear();
    }

    selectedPadNumberLineEdit->clear();
    selectedPadOrientationComboBox->setCurrentIndex(0);
    selectedPadTypeComboBox->setCurrentIndex(0);
    selectedPadXLineEdit->clear();
    selectedPadYLineEdit->clear();

    addPadNumberLineEdit->setText(str.setNum(package.pads.size() + 1));
    addPadOrientationComboBox->setCurrentIndex(0);
    addPadTypeComboBox->setCurrentIndex(0);
    if (package.pads.empty()) {
        addPadXLineEdit->setText("0");
        addPadYLineEdit->setText("0");
    }
    else {
        addPadXLineEdit->clear();
        addPadYLineEdit->clear();
    }

    addPadsDxLineEdit->clear();
    addPadsDyLineEdit->clear();
    addPadsFirstLineEdit->setText(str.setNum(package.pads.size() + 1));
    if (package.pads.empty()) {
        addPadsFirstXLineEdit->setText("0");
        addPadsFirstYLineEdit->setText("0");
    }
    else {
        addPadsFirstXLineEdit->clear();
        addPadsFirstYLineEdit->clear();
    }
    addPadsLastLineEdit->clear();
    addPadsOrientationComboBox->setCurrentIndex(0);
    addPadsTypeComboBox->setCurrentIndex(0);

    deletePadsFirstLineEdit->clear();
    deletePadsLastLineEdit->clear();

    ellipsesLineEdit->setText(str.setNum(package.ellipses.size()));
    linesLineEdit->setText(str.setNum(package.lines.size()));
    padsLineEdit->setText(str.setNum(package.pads.size()));
}

void PackageEditor::updatePackage()
{
    const QString warningMessage = "Parameter value error";
    bool ok[16];
    int dx;
    int dy;
    int first;
    int firstX;
    int firstY;
    int last;
    int radioButtonNumber = READ_ONLY_MODE;
    Ellipse ellipse;
    Line line;
    Pad pad;
    QString str;

    for (int i = 0; i < radioButtons; i++) {
        if (radioButton[i]->isChecked()) {
            radioButtonNumber = i;
            break;
        }
    }

    switch (radioButtonNumber) {
    case ADD_ELLIPSE:
        ellipse.h = addEllipseHLineEdit->text().toInt(&ok[0]);
        ellipse.w = addEllipseWLineEdit->text().toInt(&ok[1]);
        ellipse.x = addEllipseXLineEdit->text().toInt(&ok[2]);
        ellipse.y = addEllipseYLineEdit->text().toInt(&ok[3]);
        if (ok[0] && ok [1] && ok[2] && ok[3])
            package.ellipses.push_back(ellipse);
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case ADD_LINE:
        line.x1 = addLineX1LineEdit->text().toInt(&ok[0]);
        line.x2 = addLineX2LineEdit->text().toInt(&ok[1]);
        line.y1 = addLineY1LineEdit->text().toInt(&ok[2]);
        line.y2 = addLineY2LineEdit->text().toInt(&ok[3]);
        if (ok[0] && ok [1] && ok[2] && ok[3])
            package.lines.push_back(line);
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case ADD_PAD:
        pad.number = package.pads.size() + 1;
        pad.orientation = 0;  // "Up"
        if (addPadOrientationComboBox->currentText() == "Right")
            pad.orientation = 1;
        pad.typeNumber = addPadTypeComboBox->currentText().toInt(&ok[0]);
        if (package.pads.empty()) {
            pad.x = 0;
            pad.y = 0;
            ok[1] = true;
            ok[2] = true;
        }
        else {
            pad.x = addPadXLineEdit->text().toInt(&ok[1]);
            pad.y = addPadYLineEdit->text().toInt(&ok[2]);
        }
        if (ok[0] && ok [1] && ok[2] && pad.typeNumber < package.padTypesParams.size()) {
            ok[3] = true;
            for (auto p : package.pads)
                if (pad.x == p.x && pad.y == p.y)
                    ok[3] = false;
            if (!ok[3]) {
                QMessageBox::warning(this, tr("Error"), "Pad already exists in this place");
                break;
            }
            pad.diameter = package.padTypesParams[pad.typeNumber].diameter;
            pad.height = package.padTypesParams[pad.typeNumber].height;
            pad.innerDiameter = package.padTypesParams[pad.typeNumber].innerDiameter;
            pad.net = 0;
            pad.width = package.padTypesParams[pad.typeNumber].width;
            package.pads.push_back(pad);
            addPadNumberLineEdit->setText(str.setNum(package.pads.size() + 1));
            addPadsFirstLineEdit->setText(str.setNum(package.pads.size() + 1));
            addPadXLineEdit->setReadOnly(false);
            addPadYLineEdit->setReadOnly(false);
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case ADD_PADS:
        dx = addPadsDxLineEdit->text().toInt(&ok[0]);
        dy = addPadsDyLineEdit->text().toInt(&ok[1]);
        first = package.pads.size() + 1;
        if (package.pads.empty()) {
            firstX = 0;
            firstY = 0;
            ok[2] = true;
            ok[3] = true;
        }
        else {
            firstX = addPadsFirstXLineEdit->text().toInt(&ok[2]);
            firstY = addPadsFirstYLineEdit->text().toInt(&ok[3]);
        }
        last = addPadsLastLineEdit->text().toInt(&ok[4]);
        pad.orientation = 0;  // "Up"
        if (addPadsOrientationComboBox->currentText() == "Right")
            pad.orientation = 1;
        pad.typeNumber = addPadsTypeComboBox->currentText().toInt(&ok[5]);
        if (ok[0] && ok[1] && ok[2] && ok[3] && ok[4] && ok[5] &&
            (abs(dx) > 0 || abs(dy) > 0) && first < last && last <= maxPads &&
            pad.typeNumber < package.padTypesParams.size()) {
            ok[6] = true;
            for (int i = first; i <= last; i++) {
                pad.x = firstX + i * dx;
                pad.y = firstY + i * dy;
                for (auto p : package.pads)
                    if (pad.x == p.x && pad.y == p.y)
                        ok[6] = false;
            }
            if (!ok[6]) {
                QMessageBox::warning(this, tr("Error"), "Pad already exists in this place");
                break;
            }
            for (int i = first; i <= last; i++) {
                pad.diameter = package.padTypesParams[pad.typeNumber].diameter;
                pad.height = package.padTypesParams[pad.typeNumber].height;
                pad.innerDiameter = package.padTypesParams[pad.typeNumber].innerDiameter;
                pad.net = 0;
                pad.number = i;
                pad.width = package.padTypesParams[pad.typeNumber].width;
                pad.x = firstX + i * dx;
                pad.y = firstY + i * dy;
                package.pads.push_back(pad);
            }
            addPadNumberLineEdit->setText(str.setNum(package.pads.size() + 1));
            addPadsFirstLineEdit->setText(str.setNum(package.pads.size() + 1));
            addPadsFirstXLineEdit->setReadOnly(false);
            addPadsFirstYLineEdit->setReadOnly(false);
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case BORDER:
        tmpPackage.border.bottomY = borderBottomLineEdit->text().toInt(&ok[0]);
        tmpPackage.border.leftX = borderLeftXLineEdit->text().toInt(&ok[1]);
        tmpPackage.border.rightX = borderRightXLineEdit->text().toInt(&ok[2]);
        tmpPackage.border.topY = borderTopYLineEdit->text().toInt(&ok[3]);
        if (ok[0] && ok [1] && ok[2] && ok[3] &&
            tmpPackage.border.leftX < tmpPackage.border.rightX) {
            if (tmpPackage.border.bottomY > tmpPackage.border.topY)
                package.border = tmpPackage.border;
            else {
                QMessageBox::warning(this, tr("Error"), "topY should be less than bottomY "
                                     "in this coordinate system");
                break;
            }
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case DELETE_ELLIPSES:
        first = deleteEllipsesFirstLineEdit->text().toInt(&ok[0]);
        last = package.ellipses.size();
        if (ok[0] && first > 0 && first <= last) {
            package.ellipses.resize(first - 1);
            deleteEllipsesFirstLineEdit->clear();
            deleteEllipsesLastLineEdit->setText(str.setNum(package.ellipses.size()));
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case DELETE_LINES:
        first = deleteLinesFirstLineEdit->text().toInt(&ok[0]);
        last = package.lines.size();
        if (ok[0] && first > 0 && first <= last) {
            package.lines.resize(first - 1);
            deleteLinesFirstLineEdit->clear();
            deleteLinesLastLineEdit->setText(str.setNum(package.lines.size()));
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case DELETE_PADS:
        first = deletePadsFirstLineEdit->text().toInt(&ok[0]);
        last = package.pads.size();
        if (ok[0] && first > 0 && first <= last) {
            package.pads.resize(first - 1);
            deletePadsFirstLineEdit->clear();
            deletePadsLastLineEdit->setText(str.setNum(package.pads.size()));
            addPadNumberLineEdit->setText(str.setNum(package.pads.size() + 1));
            addPadsFirstLineEdit->setText(str.setNum(package.pads.size() + 1));
            if (package.pads.empty()) {
                addPadXLineEdit->setText("0");
                addPadYLineEdit->setText("0");
                addPadsFirstXLineEdit->setText("0");
                addPadsFirstYLineEdit->setText("0");
            }
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case NAME:
        package.name = nameLineEdit->text();
        break;
    case PAD_TYPES:
        tmpPackage.padTypesParams.clear();
        for (int i = 0; i < maxPadTypes; i++) {
            QString text = padTypeShapeComboBox[i]->currentText();
            int d = 0;
            int h = 0;
            int inD = 0;
            int w = 0;
            if (text == "Rectangle") {
                w = padTypeLineEdit[i][0]->text().toInt(&ok[0]);
                if (w <= 0)
                    ok[0] = false;
                h = padTypeLineEdit[i][1]->text().toInt(&ok[1]);
                if (h <= 0)
                    ok[1] = false;
                if (package.type == "DIP") {
                    inD = padTypeLineEdit[i][2]->text().toInt(&ok[2]);
                    if (inD <= 0)
                        ok[2] = false;
                }
                else
                    ok[2] = true;
                ok[3] = true;
            }
            if (text == "Rounded rectangle") {
                w = padTypeLineEdit[i][0]->text().toInt(&ok[0]);
                if (w <= 0)
                    ok[0] = false;
                h = padTypeLineEdit[i][1]->text().toInt(&ok[1]);
                if (h <= 0)
                    ok[1] = false;
                d = 2 * padTypeLineEdit[i][2]->text().toInt(&ok[2]);
                if (d <= 0)
                    ok[2] = false;
                if (package.type == "DIP") {
                    inD = padTypeLineEdit[i][3]->text().toInt(&ok[3]);
                    if (inD <= 0)
                        ok[3] = false;
                }
                else
                    ok[3] = true;
            }
            if (text == "Circle") {
                d = padTypeLineEdit[i][0]->text().toInt(&ok[0]);
                if (d <= 0)
                    ok[0] = false;
                if (package.type == "DIP") {
                    inD = padTypeLineEdit[i][1]->text().toInt(&ok[1]);
                    if (inD <= 0)
                        ok[1] = false;
                }
                else
                    ok[1] = true;
                ok[2] = true;
                ok[3] = true;
            }
            if (ok[0] && ok [1] && ok[2] && ok[3]) {
                PadTypeParams padTypeParams = {d, h, inD, w};
                tmpPackage.padTypesParams.push_back(padTypeParams);
            }
            else
                break;
        }
        if (!tmpPackage.padTypesParams.empty())
            package.padTypesParams = tmpPackage.padTypesParams;
        break;
    case READ_ONLY_MODE:
        break;
    case SELECTED_ELLIPSE:
        if (isEllipseSelected) {
            ellipse.h = selectedEllipseHLineEdit->text().toInt(&ok[0]);
            ellipse.w = selectedEllipseWLineEdit->text().toInt(&ok[1]);
            ellipse.x = selectedEllipseXLineEdit->text().toInt(&ok[2]);
            ellipse.y = selectedEllipseYLineEdit->text().toInt(&ok[3]);
            if (ok[0] && ok [1] && ok[2] && ok[3]) {
                package.ellipses[selectedEllipseIndex] = ellipse;
                selectedEllipseHLineEdit->clear();
                selectedEllipseWLineEdit->clear();
                selectedEllipseXLineEdit->clear();
                selectedEllipseYLineEdit->clear();
                isEllipseSelected = false;
            }
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case SELECTED_LINE:
        if (isLineSelected) {
            line.x1 = selectedLineX1LineEdit->text().toInt(&ok[0]);
            line.x2 = selectedLineX2LineEdit->text().toInt(&ok[1]);
            line.y1 = selectedLineY1LineEdit->text().toInt(&ok[2]);
            line.y2 = selectedLineY2LineEdit->text().toInt(&ok[3]);
            if (ok[0] && ok [1] && ok[2] && ok[3]) {
                package.lines[selectedLineIndex] = line;
                selectedLineX1LineEdit->clear();
                selectedLineX2LineEdit->clear();
                selectedLineY1LineEdit->clear();
                selectedLineY2LineEdit->clear();
                isLineSelected = false;
            }
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case SELECTED_PAD:
        if (isPadSelected) {
            pad.number = selectedPadNumberLineEdit->text().toInt(&ok[0]);
            pad.orientation = 0;  // "Up"
            if (selectedPadOrientationComboBox->currentText() == "Right")
                pad.orientation = 1;
            pad.typeNumber = selectedPadTypeComboBox->currentText().toInt(&ok[1]);
            pad.x = selectedPadXLineEdit->text().toInt(&ok[2]);
            pad.y = selectedPadYLineEdit->text().toInt(&ok[3]);
            if (ok[0] && ok [1] && ok[2] && ok[3] && pad.number >= 1) {
                package.pads[selectedPadIndex] = pad;
                selectedPadNumberLineEdit->clear();
                selectedPadOrientationComboBox->setCurrentIndex(0);
                selectedPadTypeComboBox->setCurrentIndex(0);
                selectedPadXLineEdit->clear();
                selectedPadYLineEdit->clear();
                isPadSelected = false;
            }
        }
        else
            QMessageBox::warning(this, tr("Error"), warningMessage);
        break;
    case TYPE:
        package.type = typeComboBox->currentText();
        break;
    }

    ellipsesLineEdit->setText(str.setNum(package.ellipses.size()));
    linesLineEdit->setText(str.setNum(package.lines.size()));
    padsLineEdit->setText(str.setNum(package.pads.size()));
}

void PackageEditor::updateElement()
{
    Element tmpElement(refX, refY, orientation, "", package, "");
    element = tmpElement;
}
