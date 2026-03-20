// copperbalance.cpp
// Copyright (C) 2026 Alexander Karpeko

#include "copperbalance.h"
#include <QMessageBox>

CopperBalance::CopperBalance(const Board &board, QWidget *parent):
    board(board), QDialog(parent)
{
    setupUi(this);
    setGeometry(QRect(101, 108, 300, 520));

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(runButton, SIGNAL(clicked()), this, SLOT(run()));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));

    init();
}

void CopperBalance::accept()
{
    done(QDialog::Accepted);
}

void CopperBalance::init()
{
    maxImageSize = defaultMaxImageSize;
    step = defaultStep;
    stepLineEdit->setText(QString::number(step));
    maxImageSizeLineEdit->setText(QString::number(maxImageSize));
}

bool CopperBalance::isRectangleBoard(Point &pMin, Point &pMax)
{
    const std::vector<Point> &points = board.border.points;

    pMin.clear();
    pMax.clear();

    if (points.size() < 4)
        return false;

    int minX = points[0].x;
    int maxX = points[0].x;
    int minY = points[0].y;
    int maxY = points[0].y;

    for (auto p : points) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    if (minX == maxX || minY == maxY)
        return false;

    pMin = Point(minX, minY);
    pMax = Point(maxX, maxY);

    bool isLeftTopPoint = false;
    bool isRightBottomPoint = false;

    for (auto p : points) {
        if (p == pMin) isLeftTopPoint = true;
        if (p == pMax) isRightBottomPoint = true;
    }

    if (!isLeftTopPoint || !isRightBottomPoint)
        return false;

    Line lines[4];  // rectangle lines
    lines[0] = Line(minX, minY, maxX, minY);  // top
    lines[1] = Line(maxX, minY, maxX, maxY);  // right
    lines[2] = Line(maxX, maxY, minX, maxY);  // bottom
    lines[3] = Line(minX, maxY, minX, minY);  // left

    for (uint i = 0; i < points.size(); i++) {
        int j = i + 1;
        if (i == points.size() - 1)
            j = 0;
        Line line(points[i].x, points[i].y, points[j].x, points[j].y);
        if (!line.isHorizontal() && !line.isVertical() && !line.isEmpty())
            return false;
        bool isPartOfRectangle = false;
        for (int k = 0; k < 4; k++)
            if (lines[k].hasLine(line))
                isPartOfRectangle = true;
        if (!isPartOfRectangle)
            return false;
    }

    bool isClockWiseLine;
    int maxPathLength[2] = {0};  // clock wise path: maxPathLength[0]
    int pathLength = 0;          // clock wise path: pathLength >= 0
    int perimeter = 2 * (maxX - minX + maxY - minY);

    for (uint i = 0; i < points.size(); i++) {
        int j = i + 1;
        if (i == points.size() - 1)
            j = 0;
        Line line(points[i].x, points[i].y, points[j].x, points[j].y);
        if (line.isEmpty())
            continue;
        isClockWiseLine = false;
        if ((lines[0].hasLine(line) && line.x2 > line.x1) ||
            (lines[1].hasLine(line) && line.y2 > line.y1) ||
            (lines[2].hasLine(line) && line.x2 < line.x1) ||
            (lines[1].hasLine(line) && line.y2 < line.y1))
            isClockWiseLine = true;
        if (isClockWiseLine) pathLength += line.length();
        if (!isClockWiseLine) pathLength -= line.length();
        if (pathLength > 0)
            if (pathLength > maxPathLength[0])
                maxPathLength[0] = pathLength;
        if (pathLength < 0)
            if (-pathLength > maxPathLength[1])
                maxPathLength[1] = -pathLength;
        if (maxPathLength[0] + maxPathLength[1] >= perimeter)
            return true;
    }

    return true;
}

void CopperBalance::run()
{
    bool isRectangle = false;
    double width = 0;
    double height = 0;
    Point pMin;
    Point pMax;

    isRectangle = isRectangleBoard(pMin, pMax);

    if (!isRectangle) {
        QMessageBox::information(this, tr("Copper Balance"),
            tr("Count copper balance for a rectangle board only."));
        return;
    }

    topAverageCopperArea = 0;
    bottomAverageCopperArea = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++) {
            topAverageCopperArea += topCopperArea[i][j];
            bottomAverageCopperArea += bottomCopperArea[i][j];
        }
    topAverageCopperArea /= (rows * columns);
    bottomAverageCopperArea /= (rows * columns);
}

void CopperBalance::update()
{

}
