// board.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "board.h"
#include "exceptiondata.h"
#include "pcbtypes.h"
#include <cmath>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTextStream>
#include <set>
#include <iostream>

Board::Board()
{
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    try {
        readPackageLibrary(packagesDirectory + "/" + packagesFile);
    }
    catch (ExceptionData &e) {
        QMessageBox::warning(NULL, QString("Error"), e.show());
        return;
    }

    init();
}

void Board::addPolygon()
{
    // Delete equal points
    for (uint i = 0; i < points.size(); i++)
        for (uint j = i + 1; j < points.size(); j++) {
            if (points[i] == points[j])
                points[j].x = -1;
        }
    points2.clear();
    for (uint i = 0; i < points.size(); i++)
        if (points[i].x != -1)
            points2.push_back(points[i]);

    if (points2.size() < 4) {
        points.clear();
        points2.clear();
        return;
    }

    if (layers.edit == FRONT) {
        polygon.fill = false;
        polygon.net = -1;
        polygon.points.resize(points2.size());
        std::copy(points2.begin(), points2.end(), polygon.points.begin());
        frontPolygons.push_back(polygon);
    }
    if (layers.edit == BACK) {
        polygon.fill = false;
        polygon.net = -1;
        polygon.points.resize(points2.size());
        std::copy(points2.begin(), points2.end(), polygon.points.begin());
        backPolygons.push_back(polygon);
    }
    if (layers.edit == BORDER) {
        border.fill = false;
        border.net = -1;
        border.points.resize(points2.size());
        std::copy(points2.begin(), points2.end(), border.points.begin());
    }

    points.clear();
    points2.clear();
}

void Board::addTrack()
{
    reduceSegments(track);
    if (layers.edit == FRONT)
        for (auto &t : track)
            frontSegments.push_back(t);
    if (layers.edit == BACK)
        for (auto &t : track)
            backSegments.push_back(t);
    track.clear();
    pointNumber = 0;
}

void Board::addJumper(int x, int y, int orientation)
{

}

void Board::addSegmentPoint(int x, int y, int width)
{
    int dx;
    int dy;

    if (pointNumber) {
        dx = x - point.x;
        dy = y - point.y;
        if (!dx && !dy)
            return;
        if (fabs(dx) > 2 * fabs(dy))
            y = point.y;
        else {
            if (2 * fabs(dx) < fabs(dy))
                x = point.x;
            else
                y = (dy / fabs(dy)) * fabs(dx) + point.y;
        }
        track.push_back(Segment(point.x, point.y, x, y, -1, width));
    }

    point = Point(x, y);
    pointNumber++;
}

void Board::addVia(int x, int y)
{

}

void Board::clear()
{
    frontPolygons.clear();
    backPolygons.clear();
    border.points.clear();
    frontSegments.clear();
    backSegments.clear();
    vias.clear();
    elements.clear();
    nets.clear();
    points.clear();
}

void Board::deleteJumper(int x, int y)
{

}

void Board::deleteNetSegments(int x, int y)
{
    int netNumber = -1;

    if (layers.edit == FRONT) {
        netNumber = deleteSegment(x, y, frontSegments);
        if (netNumber >= 0)
            for (auto i = frontSegments.end(); i != frontSegments.begin();) {
                --i;
                if ((*i).net == netNumber)
                    frontSegments.erase(i);
            }
    }

    if (layers.edit == BACK) {
        netNumber = deleteSegment(x, y, backSegments);
        if (netNumber >= 0)
            for (auto i = backSegments.end(); i != backSegments.begin();) {
                --i;
                if ((*i).net == netNumber)
                    backSegments.erase(i);
            }
    }
}

void Board::deletePolygon(int x, int y)
{
    if (layers.edit == FRONT)
        deletePolygon(x, y, frontPolygons);

    if (layers.edit == BACK)
        deletePolygon(x, y, backPolygons);

    if (layers.edit == BORDER) {
        int b = 0;
        for (auto p : border.points) {
            if (p.x < x && p.y < y)
                b |= 1;
            if (p.x < x && p.y > y)
                b |= 2;
            if (p.x > x && p.y < y)
                b |= 4;
            if (p.x > x && p.y > y)
                b |= 8;
        }
        if (b == 15)
            border.points.clear();
    }
}

int Board::deletePolygon(int x, int y, std::list <Polygon> &polygons)
{
    int netNumber = -1;

    for (auto i = polygons.end(); i != polygons.begin();) {
        --i;
        int b = 0;
        for (auto j = (*i).points.begin(); j != (*i).points.end(); ++j) {
            if ((*j).x < x && (*j).y < y)
                b |= 1;
            if ((*j).x < x && (*j).y > y)
                b |= 2;
            if ((*j).x > x && (*j).y < y)
                b |= 4;
            if ((*j).x > x && (*j).y > y)
                b |= 8;
        }
        if (b == 15) {
            netNumber = (*i).net;
            polygons.erase(i);
            return netNumber;
        }
    }

    return -1;
}

void Board::deleteSegment(int x, int y)
{
    if (layers.edit == FRONT)
        deleteSegment(x, y, frontSegments);

    if (layers.edit == BACK)
        deleteSegment(x, y, backSegments);
}

int Board::deleteSegment(int x, int y, std::list<Segment> &segments)
{
    double a, b;
    int dx, dy;
    int netNumber = -1;
    int w;
    int x1, x2; // min: x1, max: x2
    int y1, y2, y3;
    int yMin, yMax;

    for (auto i = segments.end(); i != segments.begin();) {
        --i;
        x1 = (*i).x1;
        x2 = (*i).x2;
        y1 = (*i).y1;
        y2 = (*i).y2;
        if (x1 > x2) {
            x1 = (*i).x2;
            x2 = (*i).x1;
            y1 = (*i).y2;
            y2 = (*i).y1;
        }
        yMin = y1;
        yMax = y2;
        if (y1 > y2) {
            yMin = y2;
            yMax = y1;
        }
        dx = x2 - x1;
        dy = y2 - y1;
        w = (*i).width / 2;
        if (x < (x1 - w) || x > (x2 + w) || y < (yMin - w) || y > (yMax + w))
            continue;
        a = 0;
        if (dx) {
            a = double(y2 - y1) / (x2 - x1);
            b = y1 - a * x1;
        }
        if (!dx || !dy) {
            netNumber = (*i).net;
            segments.erase(i);
            return netNumber;
        }
        y3 = a * x + b;
        if (fabs(y - y3) < 0.7 * w) {
            netNumber = (*i).net;
            segments.erase(i);
            return netNumber;
        }
    }

    return -1;
}

void Board::deleteVia(int x, int y)
{

}
/*
void Board::draw(QPainter &painter, double scale)
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    int n, n2, n3;
    int net;
    int number;
    int padNumber;
    int step;
    int type;
    double x1, y1, x2, y2;
    QFont serifFont("Times", 10, QFont::Normal);
    painter.setFont(serifFont);
    QBrush frontBrush(layers.color[FRONT]);
    QPen frontPen(frontBrush, 600 * scale, Qt::SolidLine,
                  Qt::RoundCap, Qt::RoundJoin);
    QString str;

    // table cell format: cell type (8 high bits), element number (8 bits),
    // pad number (8 bits), net number (8 low bits)
    // cell type: 0 - empty, 1 - pad, 2 - track, 3 - track variant, 4 - border

    // Draw table
    painter.setPen(QColor(0, 200, 0));
    for (int i = 0; i < 25; i++)
        painter.drawLine(200, 32 * i + 10, 968, 32 * i + 10);
    for (int i = 0; i < 25; i++)
        painter.drawLine(32 * i + 200, 10, 32 * i + 200, 778);

    painter.setPen(QColor(200, 0, 0));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++) {
            n = table[i][j];
            type = n >> 24 & 0xff;
            net = n & 0xff;
            //if (type == EMPTY)
            //    painter.drawText(32 * j + 200, 32 * i + 10, 32, 32,
            //                                  Qt::AlignCenter, "-");
            if (type == PAD) {
                painter.drawRect(32 * j + 202, 32 * i + 12, 28, 28);
                painter.drawText(32 * j + 200, 32 * i + 10, 32, 32,
                                  Qt::AlignCenter, str.setNum(net));
            }
            if (type == TRACK) {
                painter.drawText(32 * j + 200, 32 * i + 10, 32, 32,
                                  Qt::AlignCenter, str.setNum(net));
            }
            if (type == TRACK_VARIANT) {
                painter.drawText(32 * j + 200, 32 * i + 10, 32, 32,
                                              Qt::AlignCenter, "Tr");
                //padNumber = n >> 16 & 0xff;
                //step = n >> 8 & 0xff;
                //if (step < 4)
                //    painter.drawText(15 * j + 200, 15 * i + 10, 15, 15,
                //                     Qt::AlignCenter, str.setNum(padNumber));
            //    if (step < 100)
            //        painter.drawText(15 * j + 200, 15 * i + 10, 15, 15,
            //                         Qt::AlignCenter, str.setNum(step));
            }
            if (type == BORDER)
                painter.drawText(32 * j + 200, 32 * i + 10, 32, 32,
                                   Qt::AlignCenter, str.setNum(-1));
            if (type == BUSY)
                painter.drawText(32 * j + 200, 32 * i + 10, 32, 32,
                                              Qt::AlignCenter, "*");
           // if (type == 10) {
                //step = n >> 8 & 0xff;
                //if (step < 100)
           //     painter.drawRect(15 * j + 202, 15 * i + 12, 11, 11);
           //     painter.drawText(15 * j + 200, 15 * i + 10, 15, 15,
           //                       Qt::AlignCenter, str.setNum(net));
           // }
        }

    // Draw tracks
    for (int i = 0; i < nets.size(); i++) {
        painter.setPen(netColor[i]);
        for (int j = 0; j < maxLine; j++) {
            x1 = tracks[i][j][0];
            y1 = tracks[i][j][1];
            x2 = tracks[i][j][2];
            y2 = tracks[i][j][3];
            if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0)
                break;
            painter.drawLine(32 * x1 + 200, 32 * y1 + 10, 32 * x2 + 200, 32 * y2 + 10);
        }
    }
}
*/
/*
void Board::draw(QPainter &painter, double scale)
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    int n, n2, n3;
    int net;
    int number;
    int padNumber;
    int step;
    int type;
    int x1, y1, x2, y2;
    QFont serifFont("Times", 10, QFont::Normal);
    painter.setFont(serifFont);
    QBrush frontBrush(layers.color[FRONT]);
    QPen frontPen(frontBrush, 600 * scale, Qt::SolidLine,
                  Qt::RoundCap, Qt::RoundJoin);
    QString str;

    // table cell format: cell type (8 high bits), element number (8 bits),
    // pad number (8 bits), net number (8 low bits)
    // cell type: 0 - empty, 1 - pad, 2 - track, 3 - track variant, 4 - border

    // Draw table
    painter.setPen(QColor(0, 200, 0));
    for (int i = 0; i < 51; i++)
        painter.drawLine(200, 15 * i + 20, 950, 15 * i + 20);
    for (int i = 0; i < 51; i++)
        painter.drawLine(15 * i + 200, 20, 15 * i + 200, 770);

    painter.setPen(QColor(200, 0, 0));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++) {
            n = table[i][j];
            type = n >> 24 & 0xff;
            net = n & 0xff;
            if (type == EMPTY)
                painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
                                              Qt::AlignCenter, "-");
            if (type == PAD) {
                painter.drawRect(15 * j + 202, 15 * i + 22, 11, 11);
                painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
                                  Qt::AlignCenter, str.setNum(net));
            }
            if (type == TRACK) {
                painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
                                  Qt::AlignCenter, str.setNum(net));
            }
            //if (type == TRACK_VARIANT) {
            //    padNumber = n >> 16 & 0xff;
            //    step = n >> 8 & 0xff;
                //if (step < 4)
                //    painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
                //                     Qt::AlignCenter, str.setNum(padNumber));
            //    if (step < 100)
            //        painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
            //                         Qt::AlignCenter, str.setNum(step));
            //}
            if (type == BORDER)
                painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
                                   Qt::AlignCenter, str.setNum(-1));
            if (type == BUSY)
                painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
                                              Qt::AlignCenter, "*");
           // if (type == 10) {
                //step = n >> 8 & 0xff;
                //if (step < 100)
           //     painter.drawRect(15 * j + 202, 15 * i + 22, 11, 11);
           //     painter.drawText(15 * j + 200, 15 * i + 20, 15, 15,
           //                       Qt::AlignCenter, str.setNum(net));
           // }
        }
}
*/

void Board::draw(QPainter &painter, double scale)
{
    const QColor colors[8] = {
        QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255),
        QColor(200, 200, 0), QColor(0, 200, 200), QColor(200, 0, 200),
        QColor(150, 150, 50), QColor(50, 150, 150)
    };
    bool fill;
    int n1, n2;
    int space = polygonSpace;
    int x1, y1, x2, y2;
    int width = defaultLineWidth;
    QFont serifFont("Times", 10, QFont::Normal);
    painter.setFont(serifFont);
    QBrush frontBrush(layers.color[FRONT]);
    QBrush backBrush(layers.color[BACK]);
    QBrush borderBrush(layers.color[BORDER]);
    QBrush whiteBrush(QColor(255, 255, 255, 255));
    QPen frontPen(frontBrush, width * scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen backPen(backBrush, width * scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen borderPen(borderBrush, 500 * scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen whitePen(whiteBrush, width * scale, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    // Draw border
    if (layers.draw & (1 << BORDER)) {
        painter.setPen(borderPen);
        border.draw(painter, scale, whiteBrush);
    }

    // Draw back polygons
    if (layers.draw & (1 << BACK_POLYGON)) {
        painter.setPen(layers.color[BACK]);
        for (auto b : backPolygons)
            b.draw(painter, scale, backBrush);
    }

    // Draw segments
    if (layers.draw & (1 << BACK)) {
        fill = false;
        for (auto i = backPolygons.begin(); i != backPolygons.end(); ++i)
            if ((*i).fill)
                fill = true;
        if (fill) {
            painter.setPen(whitePen);
            for (auto b : backSegments) {
                if (b.width + 2 * space != width) {
                    whitePen.setWidth((b.width + 2 * space) * scale);
                    painter.setPen(whitePen);
                    width = b.width + 2 * space;
                }
                painter.drawLine(scale * b.x1, scale * b.y1,
                                 scale * b.x2, scale * b.y2);
            }
            if (layers.edit == BACK) {
                for (auto t : track) {
                    if (t.width + 2 * space != width) {
                        whitePen.setWidth((t.width + 2 * space) * scale);
                        painter.setPen(whitePen);
                        width = t.width + 2 * space;
                    }
                    painter.drawLine(scale * t.x1, scale * t.y1,
                                     scale * t.x2, scale * t.y2);
                }
            }
        }

        painter.setPen(backPen);
        for (auto b : backSegments) {
            if (b.width != width) {
                backPen.setWidth(b.width * scale);
                painter.setPen(backPen);
                width = b.width;
            }
            painter.drawLine(scale * b.x1, scale * b.y1,
                             scale * b.x2, scale * b.y2);
        }
        if (layers.edit == BACK) {
            for (auto t : track) {
                if (t.width != width) {
                    backPen.setWidth(t.width * scale);
                    painter.setPen(backPen);
                    width = t.width;
                }
                painter.drawLine(scale * t.x1, scale * t.y1,
                                 scale * t.x2, scale * t.y2);
            }
        }
    }

    // Draw front polygons
    if (layers.draw & (1 << FRONT_POLYGON)) {
        painter.setPen(layers.color[FRONT]);
        for (auto f : frontPolygons)
            f.draw(painter, scale, frontBrush);
    }

    // Draw segments
    if (layers.draw & (1 << FRONT)) {
        fill = false;
        for (auto i = frontPolygons.begin(); i != frontPolygons.end(); ++i)
            if ((*i).fill)
                fill = true;
        if (fill) {
            painter.setPen(whitePen);
            for (auto f : frontSegments) {
                if (f.width + 2 * space != width) {
                    whitePen.setWidth((f.width + 2 * space) * scale);
                    painter.setPen(whitePen);
                    width = f.width + 2 * space;
                }
                painter.drawLine(scale * f.x1, scale * f.y1,
                                 scale * f.x2, scale * f.y2);
            }
            if (layers.edit == FRONT) {
                for (auto t : track) {
                    if (t.width + 2 * space != width) {
                        whitePen.setWidth((t.width + 2 * space) * scale);
                        painter.setPen(whitePen);
                        width = t.width + 2 * space;
                    }
                    painter.drawLine(scale * t.x1, scale * t.y1,
                                     scale * t.x2, scale * t.y2);
                }
            }
        }

        painter.setPen(frontPen);
        for (auto f : frontSegments) {
            if (f.width != width) {
                frontPen.setWidth(f.width * scale);
                painter.setPen(frontPen);
                width = f.width;
            }
            painter.drawLine(scale * f.x1, scale * f.y1,
                             scale * f.x2, scale * f.y2);
        }
        if (layers.edit == FRONT) {
            for (auto t : track) {
                if (t.width != width) {
                    frontPen.setWidth(t.width * scale);
                    painter.setPen(frontPen);
                    width = t.width;
                }
                painter.drawLine(scale * t.x1, scale * t.y1,
                                 scale * t.x2, scale * t.y2);
            }
        }
    }

    // Draw elements
    for (auto e : elements)
        e.draw(painter, layers, scale);

    // Draw group
    //if (groupBorder.isValid()) {
    //    painter.setPen(QColor(200, 0, 0));
    //    painter.drawRect(groupBorder);
    //}

    // Draw nets
    if (showNets) {
        for (auto n : nets) {
            if (n.number < 0)
                continue;
            if (!n.number && !showGroundNets)
                continue;
            painter.setPen(colors[n.number%8]);
            for (uint i = 0; i < n.pads.size(); i++) {
                n1 = n.pads[i].x;
                n2 = n.pads[i].y;
                x2 = scale * elements[n1].pads[n2].x;
                y2 = scale * elements[n1].pads[n2].y;
                if (i)
                    painter.drawLine(x1, y1, x2, y2);
                x1 = x2;
                y1 = y2;
            }
        }
    }

    // Draw points
    if (layers.edit == FRONT || layers.edit == BACK || layers.edit == BORDER) {
        painter.setPen(layers.color[layers.edit]);
        for (uint i = 1; i < points.size(); i++)
            painter.drawLine(scale * points[i-1].x, scale * points[i-1].y,
                             scale * points[i].x, scale * points[i].y);
    }

    // Draw groups
    //for (auto g : groups)
    //    for (auto i = g.begin(); i != g.end(); ++i)
    //        if (i != g.begin())
    //            elements[(*i)].draw(painter, layers, scale);

    // Draw path
    //painter.setPen(QColor(0, 250, 0));
    //for (uint i = 0; i < pointX.size(); i++)
    //    painter.drawPoint(pointX[i], pointY[i]);
    painter.setPen(QColor(0, 250, 0));
    for (auto t : track)
        painter.drawLine(t.x1, t.y1, t.x2, t.y2);

    // Draw tracks
    //if (layers.number & (1 << FRONT)) {
    //    painter.setPen(layers.color[FRONT]);
    //    for (TrackIt i = tracks.begin(); i != tracks.end(); ++i)
    //        (*i).draw(painter);
    //}

    // Draw vias
    //if (layers.number & (1 << FRONT_VIA)) {
    //    painter.setPen(layers.color[FRONT_VIA]);
    //    for (ViaIt i = vias.begin(); i != vias.end(); ++i)
    //        (*i).draw(painter);
    //}
    //else if (layers.number & (1 << BACK_VIA)) {
    //    painter.setPen(layers.color[BACK_VIA]);
    //    for (ViaIt i = vias.begin(); i != vias.end(); ++i)
    //        (*i).draw(painter);
    //}
}

void Board::errorCheck(QString &text)
{

}

void Board::fillPolygon(int x, int y)
{
    if (layers.edit == FRONT)
        fillPolygon(x, y, frontPolygons);

    if (layers.edit == BACK)
        fillPolygon(x, y, backPolygons);
}

void Board::fillPolygon(int x, int y, std::list <Polygon> &polygons)
{
    for (auto i = polygons.begin(); i != polygons.end(); ++i) {
        int b = 0;
        for (auto j = (*i).points.begin(); j != (*i).points.end(); ++j) {
            if ((*j).x < x && (*j).y < y)
                b |= 1;
            if ((*j).x < x && (*j).y > y)
                b |= 2;
            if ((*j).x > x && (*j).y < y)
                b |= 4;
            if ((*j).x > x && (*j).y > y)
                b |= 8;
        }
        if (b == 15) {
            (*i).fill ^= 1;
            if ((*i).fill)
                (*i).net = 0;
            else
                (*i).net = -1;
        }
    }
}

void Board::getNets()
{
    std::set <int> netNumbers;

    for (auto e : elements)
        for (auto ep : e.pads)
            netNumbers.insert(ep.net);

    int size = netNumbers.size();
    nets.resize(size);

    int num = 0;
    for (auto n : netNumbers)
        nets[num++].number = n;

    for (auto &n : nets)
        for (uint i = 0; i < elements.size(); i++)
            for (uint j = 0; j < elements[i].pads.size(); j++)
                if (n.number == elements[i].pads[j].net)
                    n.pads.push_back(Point(i, j));
}

void Board::init()
{
    int r, g, b;

    for (int i = 0; i < maxNet; i++) {
        r = 100 + 32 * (i >> 4 & 0x03);
        g = 100 + 32 * (i >> 2 & 0x03);
        b = 100 + 32 * (i & 0x03);
        netColor[i] = QColor(r, g, b);
    }

    std::fill_n(table[0], rows * columns, 0);
    selectedElement = false;
    showGroundNets = false;
    showMessage = false;
    showNets = true;
    polygonSpace = defaultPolygonSpace;
}

bool Board::joinLines(int &x11, int &x12, int &x21, int &x22)
{
    int min1, max1, min2, max2;

    min1 = x11;
    max1 = x12;
    if (x12 < x11) {
        min1 = x12;
        max1 = x11;
    }
    min2 = x21;
    max2 = x22;
    if (x22 < x21) {
        min2 = x22;
        max2 = x21;
    }
    if (max1 >= min2 && max2 >= min1) {
        x11 = min1;
        if (min2 < min1)
            x11 = min2;
        x12 = max1;
        if (max2 > max1)
            x12 = max2;
        return true;
    }

    return false;
}

bool Board::joinSegments(Segment &segment1, Segment &segment2)
{
    int dx1 = segment1.x2 - segment1.x1;
    int dx2 = segment2.x2 - segment2.x1;
    int dy1 = segment1.y2 - segment1.y1;
    int dy2 = segment2.y2 - segment2.y1;
    double a1, a2, b1, b2;

    if (!dx1 && !dx2 && segment1.x1 == segment2.x1)
        return joinLines(segment1.y1, segment1.y2, segment2.y1, segment2.y2);

    if (!dy1 && !dy2 && segment1.y1 == segment2.y1)
        return joinLines(segment1.x1, segment1.x2, segment2.x1, segment2.x2);

    if (!dx1 || !dx2 || !dy1 || !dy2)
        return false;
    a1 = double(dy1) / dx1;
    a2 = double(dy2) / dx2;
    b1 = segment1.y1 - a1 * segment1.x1;
    b2 = segment2.y1 - a2 * segment2.x1;

    if (fabs(a1 - a2) < 0.1 && fabs(b1 - b2) < 0.1)
        if (joinLines(segment1.x1, segment1.x2, segment2.x1, segment2.x2)) {
             segment1.y1 = a1 * segment1.x1 + b1 + 0.1;
             segment1.y2 = a1 * segment1.x2 + b1 + 0.1;
             return true;
        }

    return false;
}

double Board::meter(double x, double y)
{
    static bool start = true;
    static double x1 = 0;
    static double y1 = 0;
    double length;

    if (start) {
        start = false;
        x1 = x;
        y1 = y;
        length = 0;
    }
    else {
        start = true;
        length = sqrt((x - x1) * (x - x1) + (y - y1) * (y - y1)) / 1000.;
    }

    return length;
}

void Board::moveElement(int x, int y)
{
    static int orientation;
    static int number;
    static QString name;
    static QString packageName;
    static QString reference;

    if (!selectedElement) {
        number = -1;
        for (auto &e : elements) {
            number++;
            if (e.exist(x, y)) {
                orientation = e.orientation;
                name = e.name;
                packageName = e.packageName;
                reference = e.reference;
                selectedElement = true;
                return;
            }
        }
    }

    if (selectedElement) {
        Element element(x, y, orientation, name, packageName, reference);
        for (uint i = 0; i < element.pads.size(); i++)
            element.pads[i].net = elements[number].pads[i].net;
        element.group = elements[number].group;
        elements[number] = element;
        selectedElement = false;
        return;
    }
}

void Board::moveElement(int number, int x, int y)
{
    static int orientation;
    static QString name;
    static QString packageName;
    static QString reference;

    orientation = elements[number].orientation;
    name = elements[number].name;
    packageName = elements[number].packageName;
    reference = elements[number].reference;

    Element element(x, y, orientation, name, packageName, reference);
    for (uint i = 0; i < element.pads.size(); i++)
        element.pads[i].net = elements[number].pads[i].net;
    element.group = elements[number].group;
    elements[number] = element;
}

void Board::moveGroup()
{
    int dx = points[2].x - points[0].x;
    int dy = points[2].y - points[0].y;
    int orientation;
    int x, y;
    QString name;
    QString packageName;
    QString reference;

    // Move elements
    for (auto &e : elements)
        if (e.inside(points[0].x, points[0].y,
                     points[1].x, points[1].y)) {
            x = e.refX + dx;
            y = e.refY + dy;
            orientation = e.orientation;
            name = e.name;
            packageName = e.packageName;
            reference = e.reference;
            Element element(x, y, orientation, name, packageName, reference);
            for (uint i = 0; i < element.pads.size(); i++)
                element.pads[i].net = e.pads[i].net;
            e = element;
        }
}

void Board::moveGroup(int x, int y, double scale)
{
    Point point(x, y);
    points.push_back(point);

    switch (points.size()) {
    case 1:
        return;
    case 2:
        groupBorder.setCoords(scale * points[0].x, scale * points[0].y,
                              (scale * points[1].x)-1, (scale * points[1].y)-1);
        if (groupBorder.isValid())
            return;
        break;
    case 3:
        moveGroup();
    }

    groupBorder.setCoords(1, 1, 0, 0);
    points.clear();
}

void Board::readFile(const QString &filename, QString &text)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw ExceptionData(filename + " open error");
    QTextStream in(&file);
    text = in.readAll();
    file.close();
}

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

// Reduce number of wires
void Board::reduceSegments(std::list <Segment> &segments)
{
    std::list <Segment>::iterator i, j;

    i = segments.end();
    while (i != segments.begin()) {
        --i;
        j = segments.end();
        --j;
        while (j != i) {
            if (joinSegments(*i, *j))
                segments.erase(j);
            --j;
        }
    }
}

bool Board::segmentNets()
{
    double a, b;
    double a2, b2;
    int dx, dy;
    int dx2, dy2;
    int newSegments;
    int unconnected;
    int w;
    int x, y;
    int xMin, xMax, xMin2, xMax2;
    int yMin, yMax, yMin2, yMax2;

    reduceSegments(frontSegments);
    reduceSegments(backSegments);

    // Set net number for segment connected to pad
    for (auto n : nets)
        for (auto np : n.pads) {
            x = elements[np.x].pads[np.y].x;
            y = elements[np.x].pads[np.y].y;
            w = elements[np.x].pads[np.y].width;
            if (elements[np.x].pads[np.y].height < w)
                w = elements[np.x].pads[np.y].height;
            w /= 4;
            for (auto &f : frontSegments) {
                dx = f.x2 - f.x1;
                dy = f.y2 - f.y1;
                if (!dx)
                    if (fabs(x - f.x1) < w &&
                        ((y + w > f.y1 && y - w < f.y2) || (y + w > f.y2 && y - w < f.y1)))
                        f.net = n.number;
                if (!dy)
                    if (fabs(y - f.y1) < w &&
                        ((x + w > f.x1 && x - w < f.x2) || (x + w > f.x2 && x - w < f.x1)))
                        f.net = n.number;
                if (dx && fabs(dx) - fabs(dy) < 0.1) {
                    a = double(dy) / (dx);
                    b = f.y1 - a * f.x1;
                    if (((x + w > f.x1 && x - w < f.x2) || (x + w > f.x2 && x - w < f.x1)) &&
                        fabs(y - a * x + b) < w)
                        f.net = n.number;
                }
            }
        }

    // Set net number for segment connected to segment
    do {
        newSegments = 0;
        for (auto i = frontSegments.begin(); i != frontSegments.end(); ++i) {
            if ((*i).net == -1)
                continue;
            xMin = (*i).x1;
            xMax = (*i).x2;
            yMin = (*i).y1;
            yMax = (*i).y2;
            dx = (*i).x2 - (*i).x1;
            dy = (*i).y2 - (*i).y1;
            if (dx && fabs(dx) - fabs(dy) < 0.1) {
                a = double(dy) / (dx);
                b = (*i).y1 - a * (*i).x1;
            }
            for (auto j = frontSegments.begin(); j != frontSegments.end(); ++j) {
                if (i == j)
                    continue;
                xMin2 = (*j).x1;
                xMax2 = (*j).x2;
                yMin2 = (*j).y1;
                yMax2 = (*j).y2;
                dx2 = (*j).x2 - (*j).x1;
                dy2 = (*j).y2 - (*j).y1;
                if ((!dx && !dx2 && xMin == xMin2 &&
                    ((yMin >= yMin2 && yMin <= yMax2) || (yMin2 >= yMin && yMin2 <= yMax))) ||
                    (!dy && !dy2 && yMin == yMin2 &&
                    ((xMin >= xMin2 && xMin <= xMax2) || (xMin2 >= xMin && xMin2 <= xMax))) ||
                    (!dx && !dy2 &&
                    (xMin >= xMin2 && xMin <= xMax2 && yMin2 >= yMin && yMin2 <= yMax)) ||
                    (!dy && !dx2 &&
                    ((yMin >= yMin2 && yMin <= yMax2) || (xMin2 >= xMin && xMin2 <= xMax)))) {
                    if ((*j).net != -1 && (*j).net != (*i).net) {
                        message = "Segment nets error\n";
                        showMessage = true;
                        return false;
                    }
                    if ((*j).net == -1) {
                        (*j).net = (*i).net;
                        newSegments++;
                        continue;
                    }
                }
                if (dx2 && fabs(dx2) - fabs(dy2) < 0.1) {
                    a2 = double(dy2) / (dx2);
                    b2 = (*j).y1 - a * (*j).x1;
                }


            }
        }
    } while (newSegments);

    unconnected = 0;
    for (auto f : frontSegments)
        if (f.net == -1)
            unconnected++;

    message = QString("front segments = %1  unconnected = %2\n")
              .arg(frontSegments.size()).arg(unconnected);
    showMessage = true;
    return true;
}

void Board::turnElement(int x, int y, int direction)
{
    enum ElementOrientation {UP, RIGHT, DOWN, LEFT};

    int orientation;
    int refX;
    int refY;
    QString name;
    QString packageName;
    QString reference;

    for (auto &e : elements)
        if (e.exist(x, y)) {
            refX = e.refX;
            refY = e.refY;
            orientation = e.orientation;
            name = e.name;
            packageName = e.packageName;
            reference = e.reference;
            if (direction == LEFT) {
                orientation--;
                if (orientation < 0)
                    orientation = 3;
            }
            if (direction == RIGHT) {
                orientation++;
                if (orientation > 3)
                    orientation = 0;
            }
            Element element(refX, refY, orientation,
                            name, packageName, reference);
            for (uint i = 0; i < element.pads.size(); i++)
                element.pads[i].net = e.pads[i].net;
            e = element;
        }
}

/*
void Board::addDevice(int nameID, int x, int y)
{
    if (nameID < 0)
        nameID = 0;
    if (nameID > deviceNames)
        nameID = deviceNames;

    Device device(nameID, x, y);
    devices[device.center] = device;
}

void Board::addElement(int elementType, int x, int y, int orientation)
{
    if (!elementType)
        return;

    Element element(elementType, x, y, orientation);
    elements[element.center] = element;
}

void Board::addJunction(int x, int y)
{
    int point = (x << 16) + y;
    junctions.insert(point);
}

void Board::addNet()
{    
    reduceWires(net);
    for (WireIt i = net.begin(); i != net.end(); ++i)
        wires.push_back(*i);
    net.clear();
    pointNumber = 0;
}

// Add net name for horizontal wire,
// name side is nearest to point: x, y
void Board::addNetName(int x, int y)
{
    static WireIt wireIt;

    if (!selectedWire)
        for (WireIt i = wires.begin(); i != wires.end(); ++i)
            if (y == (*i).y1 && (*i).y1 == (*i).y2 &&
                (insideConnected(x, y, *i) || (x == (*i).x1 || x == (*i).x2))) {
                deleteJunction(x, y);
                (*i).nameSide = 0;
                if ((*i).x2 > (*i).x1 && fabs(x - (*i).x2) < fabs(x - (*i).x1) ||
                    (*i).x1 > (*i).x2 && fabs(x - (*i).x1) < fabs(x - (*i).x2))
                    (*i).nameSide = 1;
                wireIt = i;
                value.clear();
                selectedWire = true;
                return;
            }
    if (selectedWire) {
        (*wireIt).name = value;
        selectedWire = false;
    }
}

void Board::addSymbol(int symbolType, int x, int y)
{
    Symbol symbol(symbolType, x, y);
    symbols[symbol.center] = symbol;
}

bool Board::connected(const Pin &pin, const Wire &wire)
{
    if (pin.x == wire.x1 && pin.y == wire.y1 ||
        pin.x == wire.x2 && pin.y == wire.y2)
        return true;

    if (insideConnected(pin, wire))
        return true;

    return false;
}

bool Board::connected(const Wire &wire1, const Wire &wire2)
{
    int x, y;
    int point;

    // Wire ends connected
    if (wire1.x1 == wire2.x1 && wire1.y1 == wire2.y1 ||
        wire1.x1 == wire2.x2 && wire1.y1 == wire2.y2 ||
        wire1.x2 == wire2.x1 && wire1.y2 == wire2.y1 ||
        wire1.x2 == wire2.x2 && wire1.y2 == wire2.y2)
        return true;

    // Wire end connected with other wire
    if (insideConnected(wire1.x1, wire1.y1, wire2))
        return true;
    if (insideConnected(wire1.x2, wire1.y2, wire2))
        return true;
    if (insideConnected(wire2.x1, wire2.y1, wire1))
        return true;
    if (insideConnected(wire2.x2, wire2.y2, wire1))
        return true;

    // Wires crossed
    if (wire1.x1 == wire1.x2 && wire2.y1 == wire2.y2 &&
       (wire1.x1 > wire2.x1 && wire1.x1 < wire2.x2 ||
        wire1.x1 > wire2.x2 && wire1.x1 < wire2.x1) &&
       (wire2.y1 > wire1.y1 && wire2.y1 < wire1.y2 ||
        wire2.y1 > wire1.y2 && wire2.y1 < wire1.y1)) {
        x = wire1.x1;
        y = wire2.y1;
        point = (x << 16) + y;
        if (junctions.count(point))
            return true;
    }
    if (wire1.y1 == wire1.y2 && wire2.x1 == wire2.x2 &&
       (wire1.y1 > wire2.y1 && wire1.y1 < wire2.y2 ||
        wire1.y1 > wire2.y2 && wire1.y1 < wire2.y1) &&
       (wire2.x1 > wire1.x1 && wire2.x1 < wire1.x2 ||
        wire2.x1 > wire1.x2 && wire2.x1 < wire1.x1)) {
        x = wire2.x1;
        y = wire1.y1;
        point = (x << 16) + y;
        if (junctions.count(point))
            return true;
    }

    return false;
}

// Delete element or device
void Board::deleteElement(int x, int y)
{
    for (DeviceIt i = devices.begin(); i != devices.end(); ++i)
        if ((*i).second.exist(x, y)) {
            devices.erase(i);
            return;
        }

    for (ElementIt i = elements.begin(); i != elements.end(); ++i)
        if ((*i).second.exist(x, y)) {
            elements.erase(i);
            return;
        }

    for (SymbolIt i = symbols.begin(); i != symbols.end(); ++i)
        if ((*i).second.exist(x, y)) {
            symbols.erase(i);
            return;
        }
}

void Board::deleteJunction(int x, int y)
{
    int point = (x << 16) + y;
    junctions.erase(point);
}

void Board::deleteNet(int x, int y)
{
    int netNumber = -1;

    for (WireIt i = wires.begin(); i != wires.end(); ++i)
        if (insideConnected(x, y, *i) ||
            x == (*i).x1 && y == (*i).y1 || x == (*i).x2 && y == (*i).y2) {            
            netNumber = (*i).net;
            wires.erase(i);
            deleteJunction(x, y);
            break;
        }

    if (netNumber != -1)
        for (WireIt i = wires.end(); i != wires.begin();) {
            --i;
            if ((*i).net == netNumber)
                wires.erase(i);
        }
}

void Board::deleteWire(int x, int y)
{
    for (WireIt i = wires.begin(); i != wires.end(); ++i)
        if (insideConnected(x, y, *i) ||
            x == (*i).x1 && y == (*i).y1 || x == (*i).x2 && y == (*i).y2) {
            wires.erase(i);
            deleteJunction(x, y);
            return;
        }
}

void Board::enumerate()
{
    int deviceCounter[deviceReferences] = {0};
    int elementCounter[elementReferences] = {0};
    int number;
    QString str, str2, str3;

    for (DeviceIt i = devices.begin(); i != devices.end(); ++i) {
        number = ++deviceCounter[(*i).second.referenceType];
        (*i).second.reference = deviceReference[(*i).second.type] + str.setNum(number);
        for (uint j = 0; j < (*i).second.units.size(); j++)
            (*i).second.units[j].reference = deviceReference[(*i).second.type] +
                                             str.setNum(number) + "." + str2.setNum(j+1);
    }

    for (ElementIt i = elements.begin(); i != elements.end(); ++i) {
        number = ++elementCounter[(*i).second.referenceType];
        (*i).second.reference = elementReference[(*i).second.type] + str3.setNum(number);
    }
}

// Junction insert if needed
bool Board::insideConnected(int x, int y, const Wire &wire)
{
    if ((wire.x1 == wire.x2 && x == wire.x1 &&
        (y > wire.y1 && y < wire.y2 ||
         y > wire.y2 && y < wire.y1)) ||
        (wire.y1 == wire.y2 && y == wire.y1 &&
        (x > wire.x1 && x < wire.x2 ||
         x > wire.x2 && x < wire.x1))) {
        int point = (x << 16) + y;
        junctions.insert(point);
        return true;
    }

    return 0;
}

// Junction insert if needed
bool Board::insideConnected(const Pin &pin, const Wire &wire)
{
    if ((wire.x1 == wire.x2 && pin.x == wire.x1 &&
        (pin.y > wire.y1 && pin.y < wire.y2 ||
         pin.y > wire.y2 && pin.y < wire.y1)) ||
        (wire.y1 == wire.y2 && pin.y == wire.y1 &&
        (pin.x > wire.x1 && pin.x < wire.x2 ||
         pin.x > wire.x2 && pin.x < wire.x1))) {
        int point = (pin.x << 16) + pin.y;
        junctions.insert(point);
        return true;
    }

    return 0;
}

void Board::moveDown()
{
    //centerY += 0.1 * windowSizeY / scale;
}

void Board::moveLeft()
{
    //centerX -= 0.1 * windowSizeY / scale;
}

void Board::moveRight()
{
    //centerX += 0.1 * windowSizeY / scale;
}

void Board::moveUp()
{
    //centerY -= 0.1 * windowSizeY / scale;  // equal step for x and y
}

// Reduce number of wires
void Board::reduceWires(std::list <Wire> &wires)
{
    WireIt i, j;

    i = wires.end();
    while (i != wires.begin()) {
        --i;
        j = wires.end();
        --j;
        while (j != i) {
            if (joinWires(*i, *j))
                wires.erase(j);
            --j;
        }
    }
}

void Board::setValue(int x, int y)
{
    static int center;   

    if (!selectedElement) {
        for (ElementIt i = elements.begin(); i != elements.end(); ++i)
            if ((*i).second.exist(x, y)) {
                center = (*i).second.center;
                value.clear();
                selectedElement = true;
                return;
            }
    }
    if (selectedElement) {
        Element element = elements[center];
        element.value = value;
        elements[center] = element;
        selectedElement = false;
    }
}

// Update nets and insert junctions if needed
void Board::updateNets()
{
    const int elementNumberStart = 1000;
    Pin pin;
    pins.clear();
    int connect;

    // Get all pins of devices
    for (DeviceIt i = devices.begin(); i != devices.end(); ++i)
        for (uint j = 0; j < (*i).second.pins.size(); ++j) {
            pin.reference = (*i).second.reference;
            pin.number = j + 1;
            pin.x = (*i).second.pins[j].x;
            pin.y = (*i).second.pins[j].y;
            pin.net = -1;
            pins.push_back(pin);
        }

    // Get all pins of elements
    for (ElementIt i = elements.begin(); i != elements.end(); ++i)
        for (int j = 0; j < (*i).second.pins.size(); ++j) {
            pin.reference = (*i).second.reference;
            pin.number = j + 1;
            pin.x = (*i).second.pins[j].x;
            pin.y = (*i).second.pins[j].y;
            pin.net = -1;
            pins.push_back(pin);
        }

    // Get all pins of ground
    for (SymbolIt i = symbols.begin(); i != symbols.end(); ++i) {
        pin.reference = symbolTypeString[(*i).second.type];
        pin.number = 1;
        pin.x = (*i).second.refX;
        pin.y = (*i).second.refY;
        pin.net = 0;        // ground net
        pins.push_back(pin);
    }

    reduceWires(wires);

    // Add junctions
    for (PinIt i = pins.begin(); i != pins.end(); ++i) {
        connect = 0;
        for (PinIt j = i; j != pins.end();) {
            ++j;
            if ((*i).x == (*j).x && (*i).y == (*j).y)
                connect++;
        }
        for (WireIt j = wires.begin(); j != wires.end(); ++j) {
            if ((*i).x == (*j).x1 && (*i).y == (*j).y1 ||
                (*i).x == (*j).x2 && (*i).y == (*j).y2)
                connect++;
        }
        if (connect > 1)
            addJunction((*i).x, (*i).y);
    }

    int netNumber = 1;

    for (PinIt i = pins.begin(); i != pins.end(); ++i) {
        // Set net number for pin to pin connection
        for (PinIt j = i; j != pins.end();) {
            ++j;
            if ((*i).x == (*j).x && (*i).y == (*j).y) {
                if ((*i).net == -1)
                    (*i).net = netNumber++;
                (*j).net = (*i).net;
            }
        }
        // Set net number for pin to wire connection        
        for (WireIt j = wires.begin(); j != wires.end(); ++j)
            if (connected(*i, *j)) {
                if ((*i).net == -1 && (*j).net == -1) {
                    (*i).net = netNumber++;
                    (*j).net = (*i).net;
                }
                if ((*i).net != -1 && (*j).net == -1)
                    (*j).net = (*i).net;
                if ((*i).net == -1 && (*j).net != -1)
                    (*i).net = (*j).net;
                if ((*i).net != -1 && (*j).net != -1) {
                    if ((*i).net < (*j).net)
                        (*j).net = (*i).net;
                    if ((*i).net > (*j).net)
                        (*i).net = (*j).net;
                }
            }
        // Set net number for wire to wire connection
        for (WireIt j = wires.begin(); j != wires.end(); ++j)
            for (WireIt k = wires.begin(); k != wires.end(); ++k)
                if (connected(*j, *k)) {                    
                    if ((*j).net != -1 && (*k).net == -1)
                        (*k).net = (*i).net;
                    if ((*j).net == -1 && (*k).net != -1)
                        (*j).net = (*k).net;
                    if ((*j).net != -1 && (*k).net != -1) {
                        if ((*j).net < (*k).net)
                            (*k).net = (*j).net;
                        if ((*j).net > (*k).net)
                            (*j).net = (*k).net;
                    }
                }
    }
}
*/
