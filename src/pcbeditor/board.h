// board.h
// Copyright (C) 2018 Alexander Karpeko
// Coordinate unit: 1 micrometer

#ifndef BOARD_H
#define BOARD_H

#include "element.h"
#include "layers.h"
#include "pcbtypes.h"
#include "router.h"
#include "text.h"
#include "track.h"
#include <functional>
#include <QByteArray>

const QString packagesDirectory = "../../../library/packages";
const QString packagesFile = "packages.lib";

typedef std::list<Segment> Track;

class Board
{
public:
    static constexpr int fontScale = 100;
    static constexpr int rows = 24;
    static constexpr int columns = 24;
    static constexpr int maxLine = 256;
    static constexpr int maxNet = 64;
    static constexpr int maxPad = 32;
    static constexpr int maxLines = maxLine * maxNet;
    static constexpr int maxData = 8 * maxLine * maxNet;
    static constexpr int maxStep = 2 * (rows + columns);
    static constexpr int maxTurn = maxStep;
    static constexpr int defaultLineWidth = 700;
    static constexpr int defaultPolygonSpace = 1000;
    static constexpr int defaultSolderMaskSwell = 50;

    Board();
    void addJumper(const QString &packageName, int x, int y);
    void addLineToTrack(double track[][4], int &trackLength,
                        double x1, double y1, double x2, double y2);
    void addSegmentPoint(int x, int y, int width);
    void addPoint(int x, int y);
    void addPolygon();
    void addToGroup(Group &group, int n1, int n2, int &groupNumber);
    void addTrack();
    void addVia(int x, int y, int diameter, int innerDiameter);
    void clear();
    int compareLine(int greater, int *lineIndex, int lines,
                    int coordinate, double value);
    void connectJumper(int x, int y);
    void connectPadCenter(double track[][4], int &trackLength);
    void createGroups();
    void deleteJumper(int x, int y);
    void deleteNetSegments(int x, int y);
    void deletePolygon(int x, int y);
    int deletePolygon(int x, int y, std::list<Polygon> &polygons);
    void deleteSegment(int x, int y);
    int deleteSegment(int x, int y, std::list<Segment> &segments);
    void deleteVia(int x, int y);
    void disconnectJumper(int x, int y);
    void draw(QPainter &painter, int fontSize, double scale);
    void errorCheck(QString &text);
    void extendSpace(int netNumber);
    void fillPolygon(int x, int y);
    void fillPolygon(int x, int y, std::list<Polygon> &polygons);
    void findTableBorder();
    void fromNetlist(const QByteArray &array);
    void fromJson(const QByteArray &array);
    void getLineCoordinates(double *line, double &x, double &y,
                            double &x2, double &y2);
    void getNets();
    void getNextCell(int &row2, int &col2, int row, int col, int direction);
    int getPadsOfNet(int *netPadsRow, int *netPadsCol);
    int greaterLine(int *lineIndex, int lines, int coordinate, double value);
    void increasePoligons();
    void init();
    bool joinLines(int &x11, int &x12, int &x21, int &x22);
    bool joinSegments(Segment &segment1, Segment &segment2);
    int lessLine(int *lineIndex, int lines, int coordinate, double value);
    double lineCoordinate(int coordinate, int number);
    double meter(double x, double y);
    void moveDown();
    void moveElement(int x, int y);
    void moveElement(int number, int x, int y);
    void moveGroup();
    void moveGroup(int x, int y, double scale);
    void moveLeft();
    void moveLine(int row, int col, int delta);
    // void movePad(int row, int col, int delta);
    void movePadsToTableCenter();
    void moveRight();
    void moveUp();
    bool nearestHorizontalLine(int side, int index, double &x1,
                               double &y1, double &x2, double x);
    bool nearestVerticalLine(int side, int index, double &x1,
                             double &y1, double &y2, double y);
    int netLength(const Element &element1, const Element &element2);
    bool nextCellPoint(int row, int col, int direction);
    void noRoundTurn(int x, int y);
    void orderTrackLines(double track[][4], int &trackLength);
    int packageSpace(const Element &element1, const Element &element2);
    int padSpace(const Element &element1, const Element &element2);
    void place();
    void placeElements();
    void placeGroup(const Group &group, int refX, int refY);
    void placePadsToTable();
    void readFile(const QString &filename, QString &text);
    void readJsonFile(const QString &filename, QByteArray &byteArray);
    void readPackageLibrary(const QString &libraryname);
    void readPackages(const QByteArray &byteArray);
    void reduceSegments(std::list<Segment> &segments);
    void reduceTrack(double track[][4], int &trackLength);
    void removeUnconnectedLines(double track[][4], int &trackLength,
                                int *netPadsRow, int *netPadsCol, int netPadsLength);
    void roundTurn(int x, int y, int turningRadius);
    void route();
    void routeTracks();
    // double searchCoordinate(int coordinate, int number);
    bool segmentNets();
    void setPadSteps(int padSteps[][maxPad], int netPad, int netPadsLength,
                     int *netPadsRow, int *netPadsCol);
    void setRouteBorder();
    void setTrack(double track[][4], int &trackLength,
                  int netPadsLength, int *netPadsRow, int *netPadsCol);
    void setTurnSteps(int padSteps[][maxPad], int padTurns[][maxPad], int netPad,
                      int netPadsLength, int *netPadsRow, int *netPadsCol);
    void sortLineIndex();
    bool step(int row, int col, int direction);
    int tableRoute();
    QJsonObject toJson();
    void turnElement(int x, int y, int direction);
    int waveRoute();

    bool fillPads;
    bool openMaskOnVia;
    bool selectedElement;
    bool selectedPad;
    bool showGroundNets;
    bool showMessage;
    bool showNets;
    int coordinateNumber;
    int hLines;         // number of horizontal lines
    int lineNumber;
    int minRow, maxRow;
    int minColumn, maxColumn;
    int netNumber;
    int pointNumber;
    int polygonSpace;
    int solderMaskSwell;
    int tmpTrackLength;
    int trackLines;
    int vLines;         // number of vertical lines
    Element element;
    Group group;
    Groups groups;
    Layers layers;
    Placer placer;
    Point point;
    Polygon border;
    Polygon polygon;
    QRect groupBorder;
    QString message;
    QString packageName;
    Router router;
    Segment segment;
    Track track;
    double tmpTrack[maxLines][4];
    double tracks[maxNet][maxLine][4];  // x1, y1, x2, y2
    double trackX[rows][columns];
    double trackY[rows][columns];
    int maxStep2[maxNet];
    int tmpData[maxData];
    int table[rows][columns];
    int stepTable[rows][columns];   // steps table
    int turnTable[rows][columns];   // turns table
    int hLineIndex[maxLines];       // horizontal line index sorted by y1
    int vLineIndex[maxLines];       // vertical line index sorted by x1
    int lineIndex[4][maxLines];     // index sorted by coordinate from 0 to max
    int trackLine[maxNet];          // number of lines
    QColor netColor[maxNet];
    std::list<Polygon> topPolygons;
    std::list<Polygon> bottomPolygons;
    std::list<Segment> topSegments;
    std::list<Segment> bottomSegments;
    std::list<Via> vias;
    std::vector<Element> elements;
    std::vector<Net> nets;
    std::vector<Point> points;
    std::vector<Point> points2;

    // Test data
    std::vector<int> pointX;
    std::vector<int> pointY;

private:
    void drawSegments(const std::list<Segment> &segments, QPainter &painter,
                      QPen &pen, int width, double scale, int space = 0);
    void drawSolderMask(QPainter &painter, int layer, double scale);
    bool round45DegreesTurn(std::list<Segment>::iterator it[], int tx, int ty,
                            int minTurn, int maxTurn, int turningRadius);
    bool round90DegreesTurn(std::list<Segment>::iterator it[], int tx, int ty,
                            int minTurn, int maxTurn, int turningRadius);
    bool roundCrossing(std::list<Segment>::iterator it[]);
    bool roundJoin(std::list<Segment>::iterator it[]);
    bool roundTurn2(std::list<Segment>::iterator it[], int turningRadius);
    int turnNumber(int x0, int y0, int x, int y);
};

#endif  // BOARD_H
