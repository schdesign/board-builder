// router.cpp
// Copyright (C) 2018 Alexander Karpeko

#include "board.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>

void Board::addLineToTrack(double track[][4], int &trackLength,
                           double x1, double y1, double x2, double y2)
{
    double tmp;

    if (fabs(x1 - x2) > minValue && fabs(y1 - y2) > minValue)
        return;

    if (fabs(x1 - x2) > minValue)
        if (x1 > x2) {
            tmp = x1;
            x1 = x2;
            x2 = tmp;
        }

    if (fabs(y1 - y2) > minValue)
        if (y1 > y2) {
            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }

    track[trackLength][0] = x1;
    track[trackLength][1] = y1;
    track[trackLength][2] = x2;
    track[trackLength][3] = y2;
    trackLength++;
}

void Board::addToGroup(Group &group, int n1, int n2, int &groupNumber)
{
    if (group.empty()) {
        group.push_back(groupNumber);
        groupNumber++;
    }
    if (group.size() == 1) {
        group.push_back(n1);
        elements[n1].group = true;
    }
    group.push_back(n2);
    elements[n2].group = true;
}

int Board::compareLine(int greater, int *lineIndex, int lines,
                       int coordinate, double value)
{
    double minCoordinate;
    double maxCoordinate;
    int min = 0;
    int max = lines - 1;
    int n = (min + max) / 2;
    int n2;

    if (!lines)
        return -1;

    minCoordinate = lineCoordinate(coordinate, lineIndex[0]);
    maxCoordinate = lineCoordinate(coordinate, lineIndex[lines-1]);

    if (value < minCoordinate)
        return greater - 1;
    if (value > maxCoordinate)
        return lines + greater - 1;

    while (min < max) {
        n2 = lineIndex[n];
        if (value > lineCoordinate(coordinate, n2))
            min = n + 1;
        else
            max = n - 1;
        n = (min + max) / 2;
    }

    n2 = lineIndex[n];
    if (!greater && value < lineCoordinate(coordinate, n2))
        return n - 1;
    if (greater && value < lineCoordinate(coordinate, n2))
        return n;

    return n + greater;
}

void Board::connectPadCenter(double track[][4], int &trackLength)
{
    double x, y;
    double x11, y11, x12, y12;
    int netPadsRow[maxPad];
    int netPadsCol[maxPad];
    int netPadsLength = getPadsOfNet(netPadsRow, netPadsCol);

    for (int i = 0; i < netPadsLength; i++) {
        x = 0.5 + netPadsCol[i];
        y = 0.5 + netPadsRow[i];
        for (int j = 0; j < trackLength; j++) {
            getLineCoordinates(track[j], x11, y11, x12, y12);
            if (floor(x) == floor(x11) && floor(y) == floor(y11) &&
               (fabs(x - x11) > minValue || fabs(y - y11) > minValue)) {
                addLineToTrack(track, trackLength, x, y, x11, y11);
                break;
            }
            if (floor(x) == floor(x12) && floor(y) == floor(y12) &&
               (fabs(x - x12) > minValue || fabs(y - y12) > minValue)) {
                addLineToTrack(track, trackLength, x, y, x12, y12);
                break;
            }
        }
    }
}

void Board::createGroups()
{
    const int connectLevel = 2;
    const int groupStart = 0x10000;
    const int maxElements = 10;
    const int maxGroups = 10;
    int connect;
    int groupNumber = groupStart;
    int n;
    std::multimap <int, int> tmpMultimap;   // pads number, element number
    std::vector <int> bigElements;          // big element numbers

    for (auto &e : elements)
        e.group = false;

    // Create groups for parallel 2 pads elements    
    for (uint i = 0; i < elements.size(); i++) {
        if (elements[i].pads.size() != 2)
            continue;
        group.clear();
        for (uint j = i + 1; j < elements.size(); j++) {
            if (elements[j].pads.size() != 2 || elements[j].group)
                continue;
            if ((elements[i].pads[0].net == elements[j].pads[0].net &&
                 elements[i].pads[1].net == elements[j].pads[1].net) ||
                (elements[i].pads[0].net == elements[j].pads[1].net &&
                 elements[i].pads[1].net == elements[j].pads[0].net))
                addToGroup(group, i, j, groupNumber);
        }
        if (!group.empty())
            groups.push_back(group);
    }

    // Sort more then 2 pads elements by pads number
    for (uint i = 0; i < elements.size(); i++) {
        n = elements[i].pads.size();
        if (n > 2)
            tmpMultimap.insert(std::make_pair(n, i));
    }
    for (auto i = tmpMultimap.end();; --i) {
        if (i != tmpMultimap.end())
            bigElements.push_back((*i).second);
        if (i == tmpMultimap.begin())
            break;
    }

    // Create groups, more then 2 pads elements are centers
    for (auto b : bigElements) {
        if (elements[b].group)
            continue;
        group.clear();
        for (uint j = 0; j < elements.size(); j++) {
            if (b == j || elements[j].group)
                continue;
            connect = 0;
            for (uint n1 = 0; n1 < elements[b].pads.size(); n1++)
                for (uint n2 = 0; n2 < elements[j].pads.size(); n2++) {
                    if (elements[b].pads[n1].net &&
                        elements[b].pads[n1].net == elements[j].pads[n2].net)
                        connect++;
                }
            if (connect >= connectLevel)
                addToGroup(group, b, j, groupNumber);
            if (group.size() > maxElements)
                break;
        }
        if (!group.empty())
            groups.push_back(group);
        if (groups.size() > maxGroups)
            break;
    }

    // Create groups
    for (uint i = 0; i < elements.size(); i++) {
        if (elements[i].group)
            continue;
        group.clear();
        for (uint j = 0; j < elements.size(); j++) {
            if (i == j || elements[j].group)
                continue;
            connect = 0;
            for (uint n1 = 0; n1 < elements[i].pads.size(); n1++)
                for (uint n2 = 0; n2 < elements[j].pads.size(); n2++) {
                    if (elements[i].pads[n1].net &&
                        elements[i].pads[n1].net == elements[j].pads[n2].net)
                        connect++;
                }
            if (connect)
                addToGroup(group, i, j, groupNumber);
            if (group.size() > maxElements)
                break;
        }
        if (!group.empty())
            groups.push_back(group);
        if (groups.size() > maxGroups)
            break;
    }

    // Create groups
    for (uint i = 0; i < elements.size(); i++) {
        if (elements[i].group)
            continue;
        group.clear();
        for (uint j = 0; j < elements.size(); j++) {
            if (i == j || elements[j].group)
                continue;
            connect = 0;
            for (uint n1 = 0; n1 < elements[i].pads.size(); n1++)
                for (uint n2 = 0; n2 < elements[j].pads.size(); n2++) {
                    if (elements[i].pads[n1].net == elements[j].pads[n2].net)
                        connect++;
                }
            if (connect)
                addToGroup(group, i, j, groupNumber);
            if (group.size() > maxElements)
                break;
        }
        if (!group.empty())
            groups.push_back(group);
        if (groups.size() > maxGroups)
            break;
    }
}

void Board::extendSpace(int netNumber)
{
    // table[row][column]
    // table cell format: cell type (8 high bits), element number (8 bits),
    // pad number (8 bits), net number (8 low bits)
    // cell type: 0 - empty, 1 - pad, 2 - track, 3 - track variant, 4 - border


}

void Board::findTableBorder()
{
    minRow = rows - 1;
    minColumn = columns - 1;
    maxRow = 0;
    maxColumn = 0;
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            if (table[i][j]) {
                if (i < minRow)
                    minRow = i;
                if (j < minColumn)
                    minColumn = j;
                if (i > maxRow)
                    maxRow = i;
                if (j > maxColumn)
                    maxColumn = j;
            }
    minRow--;
    minColumn--;
    maxRow++;
    maxColumn++;
    if (minRow < 0 || minRow == rows - 1)
        minRow = 0;
    if (minColumn < 0 || minColumn == columns - 1)
        minColumn = 0;
    if (maxRow > (rows - 1) || !maxRow)
        maxRow = rows - 1;
    if (maxColumn > (columns - 1) || !maxColumn)
        maxColumn = columns - 1;
}

void Board::getLineCoordinates(double *line, double &x, double &y,
                               double &x2, double &y2)
{
    x = line[0];
    y = line[1];
    x2 = line[2];
    y2 = line[3];
}

void Board::getNextCell(int &row2, int &col2, int row, int col, int direction)
{
    enum {UP, DOWN, RIGHT, LEFT};

    row2 = row;
    col2 = col;

    if (direction == UP)
    row2--;
    if (direction == DOWN)
    row2++;
    if (direction == RIGHT)
    col2++;    
    if (direction == LEFT)
    col2--;
}

int Board::getPadsOfNet(int *netPadsRow, int *netPadsCol)
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    int length = 0;
    int n;

    for (int row = minRow; row <= maxRow; row++)
        for (int col = minColumn; col <= maxColumn; col++) {
            n = table[row][col];
            if ((n >> 24 & 0xff) == PAD && (n & 0xff) == netNumber) {
                netPadsRow[length] = row;
                netPadsCol[length] = col;
                length++;
            }
        }

    return length;
}

// line > value
int Board::greaterLine(int *lineIndex, int lines, int coordinate, double value)
{
    return compareLine(1, lineIndex, lines, coordinate, value);
}

// line < value
int Board::lessLine(int *lineIndex, int lines, int coordinate, double value)
{
    return compareLine(0, lineIndex, lines, coordinate, value);
}

// Line coordinate: x1, y1, x2, y2
double Board::lineCoordinate(int coordinate, int number)
{
    int n1 = number >> 8 & 0xff;    // net number
    int n2 = number & 0xff;         // line number
    return tracks[n1][n2][coordinate];
}

void Board::moveLine(int row, int col, int delta)
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    // table cell format: cell type (8 high bits), element number (8 bits),
    // pad number (8 bits), net number (8 low bits)
    // cell type: 0 - empty, 1 - pad, 2 - track, 3 - track variant, 4 - border

    // Move up
    if (delta == 1) {
        for (int i = 1; i < row; i++)
            for (int j = 0; j < columns; j++)
                table[i-1][j] = table[i][j];
        for (int i = 0; i < columns; i++)
            table[row-1][i] = EMPTY;

        // Restore tracks
        for (int i = 0; i < columns; i++)
            if (((table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == TRACK) ||
                 (table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == PAD) ||
                 (table[row][i] >> 24 == PAD && table[row-2][i] >> 24 == TRACK)) &&
                     ((table[row][i] & 0xff) == (table[row-2][i] & 0xff)))
                table[row-1][i] = TRACK << 24 | (table[row][i] & 0xff);

        // Restore elements
        for (int i = 0; i < columns; i++)
            if ((table[row][i] >> 24 == PAD && table[row-2][i] >> 24 == PAD) &&
               ((table[row][i] >> 16 & 0xff) == (table[row-2][i] >> 16 & 0xff))) {
                if ((table[row][i] & 0xff) < (table[row-2][i] & 0xff)) {
                    table[row-1][i] = table[row-2][i];
                    table[row-2][i] = EMPTY;
                }
                else if ((table[row][i] & 0xff) > (table[row-2][i] & 0xff)) {
                    table[row-1][i] = table[row][i];
                    table[row][i] = EMPTY;
                }
            }
    }

    // Move right
    if (delta == 2 || delta == 4 || delta == 7) {
        if ((delta == 2 || delta == 7) && (table[row][col+1] >> 24 == PAD) &&
           ((table[row][col] & 0xff) == (table[row][col+1] & 0xff)))
            return;

        for (int i = 0; i < rows; i++)
            for (int j = columns - 2; j > col; j--)
                table[i][j+1] = table[i][j];
        for (int i = 1; i < rows; i++)
            table[i][col+1] = EMPTY;

        // Restore tracks
        for (int i = 0; i < rows; i++)
            if (((table[i][col] >> 24 == TRACK && table[i][col+2] >> 24 == TRACK) ||
                (table[i][col] >> 24 == TRACK && table[i][col+2] >> 24 == PAD) ||
                (table[i][col] >> 24 == PAD && table[i][col+2] >> 24 == TRACK)) &&
                ((table[i][col] & 0xff) == (table[i][col+2] & 0xff)))
                table[i][col+1] = TRACK << 24 | (table[i][col] & 0xff);
    }

    // Move down
    if (delta == 6) {
        for (int i = rows - 2; i > row; i--)
            for (int j = 0; j < columns; j++)
                table[i+1][j] = table[i][j];
        for (int i = 0; i < columns; i++)
            table[row+1][i] = EMPTY;

        // Restore tracks
        for (int i = 0; i < columns; i++)
            if (((table[row][i] >> 24 == TRACK && table[row+2][i] >> 24 == TRACK) ||
                (table[row][i] >> 24 == TRACK && table[row+2][i] >> 24 == PAD) ||
                (table[row][i] >> 24 == PAD && table[row+2][i] >> 24 == TRACK)) &&
                ((table[row][i] & 0xff) == (table[row+2][i] & 0xff)))
                table[row+1][i] = TRACK << 24 | (table[row][i] & 0xff);

        // Restore elements
        for (int i = 0; i < columns; i++)
            if ((table[row][i] >> 24 == PAD && table[row+2][i] >> 24 == PAD) &&
               ((table[row][i] >> 16 & 0xff) == (table[row+2][i] >> 16 & 0xff))) {
                if ((table[row][i] & 0xff) < (table[row+2][i] & 0xff)) {
                    table[row+1][i] = table[row+2][i];
                    table[row+2][i] = EMPTY;
                }
                else if ((table[row][i] & 0xff) > (table[row+2][i] & 0xff)) {
                    table[row+1][i] = table[row][i];
                    table[row][i] = EMPTY;
                }
            }
    }

    // Move left
    if (!delta || delta == 3 || delta == 5) {
        if ((!delta || delta == 5) && (table[row][col-1] >> 24 == PAD) &&
           ((table[row][col] & 0xff) == (table[row][col-1] & 0xff)))
            return;

        for (int i = 0; i < rows; i++)
            for (int j = 1; j < col; j++)
                table[i][j-1] = table[i][j];
        for (int i = 1; i < rows; i++)
            table[i][col-1] = EMPTY;

        // Restore tracks
        for (int i = 0; i < rows; i++)
            if (((table[i][col] >> 24 == TRACK && table[i][col-2] >> 24 == TRACK) ||
                (table[i][col] >> 24 == TRACK && table[i][col-2] >> 24 == PAD) ||
                (table[i][col] >> 24 == PAD && table[i][col-2] >> 24 == TRACK)) &&
                ((table[i][col] & 0xff) == (table[i][col-2] & 0xff)))
                table[i][col-1] = TRACK << 24 | (table[i][col] & 0xff);
    }
}

/*
void Board::movePad(int row, int col, int delta)
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    int n;
    int type;
    int element;
    int pad;
    int net;

    // Move up
    if (delta == 1) {
        n = table[row-1][col];
        type = n >> 24 & 0xff;
        element = n >> 16 & 0xff;
        pad = n >> 8 & 0xff;
        net = n & 0xff;
        for (int i = 1; i < row; i++)
            table[i-1][col] = table[i][col];
        table[row-1][col] = EMPTY;

        // Restore track
        for (int i = 0; i < columns; i++)
            if ((table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == TRACK ||
                 table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == PAD ||
                 table[row][i] >> 24 == PAD && table[row-2][i] >> 24 == TRACK) &&
                     ((table[row][i] & 0xff) == (table[row-2][i] & 0xff)))
                table[row-1][i] = TRACK << 24 | (table[row][i] & 0xff);
    }

    // Move right
    if (delta == 2 || delta == 4 || delta == 7) {
        n = table[row][col+1];
        type = n >> 24 & 0xff;
        element = n >> 16 & 0xff;
        pad = n >> 8 & 0xff;
        net = n & 0xff;
        for (int i = columns - 2; i > col; i--)
            table[row][i+1] = table[row][i];
        table[row][col+1] = EMPTY;

        // Restore track
        for (int i = 0; i < columns; i++)
            if ((table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == TRACK ||
                 table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == PAD ||
                 table[row][i] >> 24 == PAD && table[row-2][i] >> 24 == TRACK) &&
                     ((table[row][i] & 0xff) == (table[row-2][i] & 0xff)))
                table[row-1][i] = TRACK << 24 | (table[row][i] & 0xff);
    }

    // Move down
    if (delta == 6) {
        n = table[row+1][col];
        type = n >> 24 & 0xff;
        element = n >> 16 & 0xff;
        pad = n >> 8 & 0xff;
        net = n & 0xff;
        for (int i = rows - 2; i > row; i--)
             table[i+1][col] = table[i][col];
        table[row+1][col] = EMPTY;

        // Restore track
        for (int i = 0; i < columns; i++)
            if ((table[row][i] >> 24 == TRACK && table[row+2][i] >> 24 == TRACK ||
                 table[row][i] >> 24 == TRACK && table[row+2][i] >> 24 == PAD ||
                 table[row][i] >> 24 == PAD && table[row+2][i] >> 24 == TRACK) &&
                     ((table[row][i] & 0xff) == (table[row+2][i] & 0xff)))
                table[row+1][i] = TRACK << 24 | (table[row][i] & 0xff);

    }

    // Move left
    if (!delta || delta == 3 || delta == 5) {        
        n = table[row-1][col-1];
        type = n >> 24 & 0xff;
        element = n >> 16 & 0xff;
        pad = n >> 8 & 0xff;
        net = n & 0xff;
        for (int i = 1; i < col; i++)
            table[row][i-1] = table[row][i];
        table[row][col-1] = EMPTY;

        // Restore track
        for (int i = 0; i < columns; i++)
            if ((table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == TRACK ||
                 table[row][i] >> 24 == TRACK && table[row-2][i] >> 24 == PAD ||
                 table[row][i] >> 24 == PAD && table[row-2][i] >> 24 == TRACK) &&
                     ((table[row][i] & 0xff) == (table[row-2][i] & 0xff)))
                table[row-1][i] = TRACK << 24 | (table[row][i] & 0xff);
    }
}
*/

void Board::movePadsToTableCenter()
{
    int deltaColumn;
    int deltaRow;

    deltaRow = (maxRow + minRow - rows) / 2;
    if (deltaRow > 0)       // move up
        for (int i = minRow; i <= maxRow; i++)
            for (int j = minColumn; j <= maxColumn; j++) {
                table[i-deltaRow][j] = table[i][j];
                table[i][j] = 0;
            }
    if (deltaRow < 0)       // move down
        for (int i = maxRow; i >= minRow; i--)
            for (int j = minColumn; j <= maxColumn; j++) {
                table[i-deltaRow][j] = table[i][j];
                table[i][j] = 0;
            }

    findTableBorder();

    deltaColumn = (maxColumn + minColumn - columns) / 2;
    if (deltaColumn > 0)    // move left
        for (int j = minColumn; j <= maxColumn; j++)
            for (int i = minRow; i <= maxRow; i++) {
                table[i][j-deltaColumn] = table[i][j];
                table[i][j] = 0;
            }
    if (deltaColumn < 0)    // move right
        for (int j = maxColumn; j >= minColumn; j--)
            for (int i = minRow; i <= maxRow; i++) {
                table[i][j-deltaColumn] = table[i][j];
                table[i][j] = 0;
            }

    findTableBorder();
}

// Find nearest horizontal line
// Upper line: side = 0, lower line: side = 1
bool Board::nearestHorizontalLine(int side, int index, double &x1,
                                  double &y1, double &x2, double x)
{
    double y2;
    int j = -1; // upper line
    int n, n1, n2;

    if (side)   // lower line
        j = 1;

    for (int i = index; i >= 0 && i < hLines; i += j) {
        n = hLineIndex[i];
        n1 = n >> 8 & 0xff; // net number
        n2 = n & 0xff;      // line number
        getLineCoordinates(tracks[n1][n2], x1, y1, x2, y2);
        if (x1 < x + minValue && x2 > x - minValue)
            return true;
    }

    return false;
}

// Find nearest vertical line
// Left line: side = 0, right line: side = 1
bool Board::nearestVerticalLine(int side, int index, double &x1,
                                double &y1, double &y2, double y)
{
    double x2;
    int j = -1; // left line
    int n, n1, n2;

    if (side)   // right line
        j = 1;

    for (int i = index; i >= 0 && i < vLines; i += j) {
        n = vLineIndex[i];
        n1 = n >> 8 & 0xff; // net number
        n2 = n & 0xff;      // line number
        getLineCoordinates(tracks[n1][n2], x1, y1, x2, y2);
        if (y1 < y + minValue && y2 > y - minValue)
            return true;
    }

    return false;
}

int Board::netLength(const Element &element1, const Element &element2)
{
    int dx, dy;
    int length = 0;

    for (auto &e1 : element1.pads)
        for (auto &e2 : element2.pads)
            if (e1.net == e2.net) {
                dx = fabs(e2.x - e1.x);
                dy = fabs(e2.y - e1.y);
                length += sqrt(dx * dx + dy * dy);
            }

    return length;
}

// Current cell point: row, col
// Direction: up, down, right, left
// Find next cell point: x3, y3
bool Board::nextCellPoint(int row, int col, int direction)
{
    enum {UP, DOWN, RIGHT, LEFT};
    enum {X1, Y1, X2, Y2};

    bool nextCell = false;
    double x, y;
    double x1 = 0, x2 = 0, x3 = 0;
    double y1 = 0, y2 = 0, y3 = 0;
    double x11, y11, x12, y12;  // 1st line
    double x21, y21, x22, y22;  // 2nd line
    double x31, y31, x32, y32;  // 3rd line
    double x41, y41, x42, y42;  // 4th line
    int indexX1, indexY1;
    int n, n1, n2;
    int row2, col2;
    int side;

    if (row < 0 || row >= rows || col < 0 || col >= columns)
        return false;

    x = trackX[row][col];
    y = trackY[row][col];

    if (y < row + minValue || y > row + 1 - minValue ||
        x < col + minValue || x > col + 1 - minValue)
        return false;

    getNextCell(row2, col2, row, col, direction);

    if (row2 < 0 || row2 >= rows || col2 < 0 || col2 >= columns)
        return false;

    switch (direction) {
    case UP:
        // Find nearest horizontal line: lineY < y - minValue
        indexY1 = lessLine(hLineIndex, hLines, Y1, y - minValue);
        if (!hLines || indexY1 < 0) {
            x3 = x;
            y3 = floor(y) - 0.5;
            nextCell = true;
            break;
        }
        // Check upper horizontal lines
        for (int i = indexY1; i >= 0; i--) {
            n = hLineIndex[i];
            n1 = n >> 8 & 0xff; // net number
            n2 = n & 0xff;      // line number
            getLineCoordinates(tracks[n1][n2], x11, y11, x12, y12);
            if (!nextCell && (y11 < floor(y) || (!i && (x12 < x || x11 > x)))) {
                x3 = x;
                nextCell = true;    // path to next cell is open
            }
            if (!nextCell && i && (x12 < x || x11 > x)) // line don't close path
                continue;
            if (!nextCell && x11 < floor(x) && x12 > ceil(x))   // line close path
                return false;
            if (!nextCell) {
                for (int j = 0; j < trackLine[n1]; j++) {   // check lines of net
                    getLineCoordinates(tracks[n1][j], x21, y21, x22, y22);
                    if (fabs(x21 - x22) > minValue) // line is horizontal
                        continue;
                    side = -1;
                    if (x11 > floor(x) && x11 < ceil(x) &&
                        fabs(x11 - x22) < minValue && fabs(y11 - y22) < minValue) {
                        // Find nearest left vertical line: lineX < x22 - minValue
                        indexX1 = lessLine(vLineIndex, vLines, X1, x22 - minValue);
                        side = 0;
                    }
                    if (x12 > floor(x) && x12 < ceil(x) &&
                        fabs(x12 - x22) < minValue && fabs(y12 - y22) < minValue) {
                        // Find nearest right vertical line: lineX > x22 + minValue
                        indexX1 = greaterLine(vLineIndex, vLines, X1, x22 + minValue);
                        side = 1;
                    }
                    if (side >= 0) {
                        n = nearestVerticalLine(side, indexX1, x31, y31, y32, floor(y));
                        if (n && x31 > floor(x) && x31 < ceil(x)) {
                            x2 = 0.5 * (x22 + x31);
                            x3 = x2;
                            y2 = y;
                            if (y32 < y + minValue) {
                                if (side)
                                    indexX1 = greaterLine(vLineIndex, vLines, X1, x + minValue);
                                else
                                    indexX1 = lessLine(vLineIndex, vLines, X1, x - minValue);
                                n = nearestVerticalLine(side, indexX1, x41, y41, y42, y);
                                if (n && x41 > floor(x) && x41 < ceil(x)) {
                                    x1 = x;
                                    y1 = 0.5 * (y22 + y41);
                                    y2 = y1;
                                }
                            }
                        }
                        else {
                            y2 = y;
                            if (side)
                                indexX1 = greaterLine(vLineIndex, vLines, X1, x + minValue);
                            else
                                indexX1 = lessLine(vLineIndex, vLines, X1, x - minValue);
                            n = nearestVerticalLine(side, indexX1, x41, y41, y42, ceil(y));
                            if (n && x41 > floor(x) && x41 < ceil(x)) {
                                if (y41 < y + minValue) {
                                    x1 = x;
                                    y1 = 0.5 * (y22 + y41);
                                    y2 = y1;
                                }
                            }
                            if (side)
                                x2 = 0.5 * (x22 + ceil(x22));
                            else
                                x2 = 0.5 * (x22 + floor(x22));
                            x3 = x2;
                        }
                        nextCell = true;
                        break;
                    }
                }
                if (side == -1)
                    return false;
            }
            if (nextCell) { // find y3
                if ((y11 < floor(y) - 0.5 - minValue) ||
                    (!i && (x11 > x3 + minValue || x12 < x3 - minValue))) {
                    y3 = floor(y) - 0.5;
                    break;
                }
                if (y11 > floor(y) - 0.5 - minValue && x11 < x3 + minValue && x12 > x3 - minValue) {
                    y3 = 0.5 * (y11 + ceil(y11));
                    break;
                }
                continue;
            }
        }
        break;
    case DOWN:
        // Find nearest horizontal line: lineY > y + minValue
        indexY1 = greaterLine(hLineIndex, hLines, Y1, y + minValue);       
        if (!hLines || indexY1 >= hLines) {
            x3 = x;
            y3 = ceil(y) + 0.5;
            nextCell = true;
            break;
        }
        // Check lower horizontal lines
        for (int i = indexY1; i < hLines; i++) {
            n = hLineIndex[i];
            n1 = n >> 8 & 0xff; // net number
            n2 = n & 0xff;      // line number
            getLineCoordinates(tracks[n1][n2], x11, y11, x12, y12);
            if (!nextCell && (y11 > ceil(y) || (i == hLines - 1 && (x12 < x || x11 > x)))) {
                x3 = x;
                nextCell = true;    // path to next cell is open
            }
            if (!nextCell && i != hLines - 1 && (x12 < x || x11 > x))   // line don't close path
                continue;
            if (!nextCell && x11 < floor(x) && x12 > ceil(x))   // line close path
                return false;
            if (!nextCell) {
                for (int j = 0; j < trackLine[n1]; j++) {   // check lines of net
                    getLineCoordinates(tracks[n1][j], x21, y21, x22, y22);
                    if (fabs(x21 - x22) > minValue) // line is horizontal
                        continue;
                    side = -1;
                    if (x11 > floor(x) && x11 < ceil(x) &&
                        fabs(x11 - x21) < minValue && fabs(y11 - y21) < minValue) {
                        // Find nearest left vertical line: lineX < x21 - minValue
                        indexX1 = lessLine(vLineIndex, vLines, X1, x21 - minValue);
                        side = 0;
                    }
                    if (x12 > floor(x) && x12 < ceil(x) &&
                        fabs(x12 - x21) < minValue && fabs(y12 - y21) < minValue) {
                        // Find nearest right vertical line: lineX > x21 + minValue
                        indexX1 = greaterLine(vLineIndex, vLines, X1, x21 + minValue);
                        side = 1;
                    }
                    if (side >= 0) {
                        n = nearestVerticalLine(side, indexX1, x31, y31, y32, ceil(y));
                        if (n && x31 > floor(x) && x31 < ceil(x)) {
                            x2 = 0.5 * (x22 + x31);
                            x3 = x2;
                            y2 = y;
                            if (y31 > y - minValue) {
                                if (side)
                                    indexX1 = greaterLine(vLineIndex, vLines, X1, x + minValue);
                                else
                                    indexX1 = lessLine(vLineIndex, vLines, X1, x - minValue);
                                n = nearestVerticalLine(side, indexX1, x41, y41, y42, y);
                                if (n && x41 > x && x41 < ceil(x)) {
                                    x1 = x;
                                    y1 = 0.5 * (y22 + y41);
                                    y2 = y1;
                                }
                            }
                        }
                        else {
                            y2 = y;
                            if (side)
                                indexX1 = greaterLine(vLineIndex, vLines, X1, x + minValue);
                            else
                                indexX1 = lessLine(vLineIndex, vLines, X1, x - minValue);
                            n = nearestVerticalLine(side, indexX1, x41, y41, y42, ceil(y));
                            if (n && x41 > floor(x) && x41 < ceil(x)) {
                                if (y41 < y + minValue) {
                                    x1 = x;
                                    y1 = 0.5 * (y22 + y41);
                                    y2 = y1;
                                }
                            }
                            if (side)
                                x2 = 0.5 * (x22 + ceil(x22));
                            else
                                x2 = 0.5 * (x22 + floor(x22));
                            x3 = x2;
                        }
                        nextCell = true;
                        break;
                    }
                }
                if (side == -1)
                    return false;
            }
            if (nextCell) { // find y3
                if ((y11 > ceil(y) + 0.5 + minValue) ||
                    (i == hLines - 1 && (x11 > x3 + minValue || x12 < x3 - minValue))) {
                    y3 = ceil(y) + 0.5;
                    break;
                }
                if (y11 < ceil(y) + 0.5 + minValue && x11 < x3 + minValue && x12 > x3 - minValue) {
                    y3 = 0.5 * (y11 + floor(y11));
                    break;
                }
                continue;
            }
        }
        break;
    case RIGHT:
        // Find nearest vertical line: lineX > x + minValue
        indexX1 = greaterLine(vLineIndex, vLines, X1, x + minValue);
        if (!vLines || indexX1 >= vLines) {
            x3 = ceil(x) + 0.5;
            y3 = y;
            nextCell = true;
            break;
        }
        // Check right vertical lines
        for (int i = indexX1; i < vLines; i++) {
            n = vLineIndex[i];
            n1 = n >> 8 & 0xff; // net number
            n2 = n & 0xff;      // line number
            getLineCoordinates(tracks[n1][n2], x11, y11, x12, y12);
            if (!nextCell && (x11 > ceil(x) || (i == vLines - 1 && (y12 < y || y11 > y)))) {
                y3 = y;
                nextCell = true;    // path to next cell is open
            }
            if (!nextCell && i != vLines - 1 && (y12 < y || y11 > y))   // line don't close path
                continue;
            if (!nextCell && y11 < floor(y) && y12 > ceil(y))   // line close path
                return false;
            if (!nextCell) {
                for (int j = 0; j < trackLine[n1]; j++) {   // check lines of net
                    getLineCoordinates(tracks[n1][j], x21, y21, x22, y22);
                    if (fabs(y21 - y22) > minValue) // line is vertical
                        continue;
                    side = -1;
                    if (y11 > floor(y) && y11 < ceil(y) &&
                        fabs(x11 - x21) < minValue && fabs(y11 - y21) < minValue) {
                        // Find nearest upper horizontal line: lineY < y21 - minValue
                        indexY1 = lessLine(hLineIndex, hLines, Y1, y21 - minValue);
                        side = 0;
                    }
                    if (y12 > floor(y) && y12 < ceil(y) &&
                        fabs(x12 - x21) < minValue && fabs(y12 - y21) < minValue) {
                        // Find nearest lower horizontal line: lineY > y21 + minValue
                        indexY1 = greaterLine(hLineIndex, hLines, Y1, y21 + minValue);
                        side = 1;
                    }
                    if (side >= 0) {
                        n = nearestHorizontalLine(side, indexY1, x31, y31, x32, ceil(x));
                        if (n && y31 > floor(y) && y31 < ceil(y)) {
                            y2 = 0.5 * (y22 + y31);
                            y3 = y2;
                            x2 = x;
                            if (x31 > x - minValue) {
                                if (side)
                                    indexY1 = greaterLine(hLineIndex, hLines, Y1, y + minValue);
                                else
                                    indexY1 = lessLine(hLineIndex, hLines, Y1, y - minValue);
                                n = nearestHorizontalLine(side, indexY1, x41, y41, x42, x);
                                if (n && y41 > y && y41 < ceil(y)) {
                                    y1 = y;
                                    x1 = 0.5 * (x22 + x41);
                                    x2 = x1;
                                }
                            }
                        }
                        else {
                            if (side) {
                                x2 = x;
                                if (side)
                                    indexY1 = greaterLine(hLineIndex, hLines, Y1, y + minValue);
                                else
                                    indexY1 = lessLine(hLineIndex, hLines, Y1, y - minValue);
                                n = nearestHorizontalLine(side, indexY1, x41, y41, x42, ceil(x));
                                if (n && y41 > floor(y) && y41 < ceil(y)) {
                                    if (x41 < x + minValue) {
                                        y1 = y;
                                        x1 = 0.5 * (x22 + x41);
                                        x2 = x1;
                                    }
                                }
                            }
                            if (!side)
                                y2 = 0.5 * (y22 + floor(y22));
                            if (side)
                                y2 = 0.5 * (y22 + ceil(y22));
                            y3 = y2;
                        }
                        nextCell = true;
                        break;
                    }
                }
                if (side == -1)
                    return false;
            }
            if (nextCell) { // find x3
                if ((x11 > ceil(x) + 0.5 + minValue) ||
                    (i == vLines - 1 && (y11 > y3 + minValue || y12 < y3 - minValue))) {
                    x3 = ceil(x) + 0.5;
                    break;
                }
                if (x11 < ceil(x) + 0.5 + minValue && y11 < y3 + minValue && y12 > y3 - minValue) {
                    x3 = 0.5 * (x11 + floor(x11));
                    break;
                }
                continue;
            }
        }
        break;
    case LEFT:
        // Find nearest vertical line: lineX < x - minValue
        indexX1 = lessLine(vLineIndex, vLines, X1, x - minValue);
        if (!vLines  || indexX1 < 0) {
            x3 = floor(x) - 0.5;
            y3 = y;
            nextCell = true;
            break;
        }        
        // Check left vertical lines
        for (int i = indexX1;  i >= 0; i--) {
            n = vLineIndex[i];
            n1 = n >> 8 & 0xff; // net number
            n2 = n & 0xff;      // line number
            getLineCoordinates(tracks[n1][n2], x11, y11, x12, y12);
            if (!nextCell && (x11 < floor(x) || (!i && (y12 < y || y11 > y)))) {
                y3 = y;
                nextCell = true;    // path to next cell is open
            }
            if (!nextCell && i && (y12 < y || y11 > y)) // line don't close path
                continue;
            if (!nextCell && y11 < floor(y) && y12 > ceil(y))   // line close path
                return false;
            if (!nextCell) {
                for (int j = 0; j < trackLine[n1]; j++) {   // check lines of net
                    getLineCoordinates(tracks[n1][j], x21, y21, x22, y22);
                    if (fabs(y21 - y22) > minValue) // line is vertical
                        continue;
                    side = -1;
                    if (y11 > floor(y) && y11 < ceil(y) &&
                        fabs(x11 - x22) < minValue && fabs(y11 - y22) < minValue) {
                        // Find nearest upper horizontal line: lineY < y22 - minValue
                        indexY1 = lessLine(hLineIndex, hLines, Y1, y22 - minValue);
                        side = 0;
                    }
                    if (y12 > floor(y) && y12 < ceil(y) &&
                        fabs(x12 - x22) < minValue && fabs(y12 - y22) < minValue) {
                        // Find nearest lower horizontal line: lineY > y22 + minValue
                        indexY1 = greaterLine(hLineIndex, hLines, Y1, y22 + minValue);
                        side = 1;
                    }
                    if (side >= 0) {
                        n = nearestHorizontalLine(side, indexY1, x31, y31, x32, floor(x));
                        if (n && y31 > floor(y) && y31 < ceil(y)) {
                            y2 = 0.5 * (y22 + y31);
                            y3 = y2;
                            x2 = x;
                            if (x32 < x + minValue) {
                                if (side)
                                    indexY1 = greaterLine(hLineIndex, hLines, Y1, y + minValue);
                                else
                                    indexY1 = lessLine(hLineIndex, hLines, Y1, y - minValue);
                                n = nearestHorizontalLine(side, indexY1, x41, y41, x42, x);
                                if (n && y41 > floor(y) && y41 < ceil(y)) {
                                    y1 = y;
                                    x1 = 0.5 * (x22 + x41);
                                    x2 = x1;
                                }
                            }
                        }
                        else {
                            x2 = x;
                            if (side)
                                indexY1 = greaterLine(hLineIndex, hLines, Y1, y + minValue);
                            else
                                indexY1 = lessLine(hLineIndex, hLines, Y1, y - minValue);
                            n = nearestHorizontalLine(side, indexY1, x41, y41, x42, ceil(x));
                            if (n && y41 > floor(y) && y41 < ceil(y)) {
                                if (x41 < x + minValue) {
                                    y1 = y;
                                    x1 = 0.5 * (x22 + x41);
                                    x2 = x1;
                                }
                            }
                            if (side)
                                y2 = 0.5 * (y22 + ceil(y22));
                            else
                                y2 = 0.5 * (y22 + floor(y22));
                            y3 = y2;
                        }
                        nextCell = true;
                        break;
                    }
                }
                if (side == -1)
                    return false;
            }
            if (nextCell) { // find x3
                if ((x11 < floor(x) - 0.5 - minValue) ||
                    (!i && (y11 > y3 + minValue || y12 < y3 - minValue))) {
                    x3 = floor(x) - 0.5;         
                    break;
                }
                if (x11 > floor(x) - 0.5 - minValue && y11 < y3 + minValue && y12 > y3 - minValue) {
                    x3 = 0.5 * (x11 + ceil(x11));
                    break;
                }
                continue;
            }
        }
        break;
    }

    // Next cell point
    if (nextCell) {        
        trackX[row2][col2] = x3;
        trackY[row2][col2] = y3;
    }

    // Add lines to track
    if (nextCell) {
        if (fabs(x1) > minValue && fabs(y1) > minValue) {
            addLineToTrack(tmpTrack, tmpTrackLength, x, y, x1, y1);
            addLineToTrack(tmpTrack, tmpTrackLength, x1, y1, x2, y2);
            addLineToTrack(tmpTrack, tmpTrackLength, x2, y2, x3, y3);
            return true;
        }
        if (fabs(x2) > minValue && fabs(y2) > minValue) {
            addLineToTrack(tmpTrack, tmpTrackLength, x, y, x2, y2);
            addLineToTrack(tmpTrack, tmpTrackLength, x2, y2, x3, y3);
            return true;
        }
        if (fabs(x3) > minValue && fabs(y3) > minValue) {
            addLineToTrack(tmpTrack, tmpTrackLength, x, y, x3, y3);
            return true;
        }
    }

    return false;
}

// Set line: x1 <= x2, y1 <= y2
void Board::orderTrackLines(double track[][4], int &trackLength)
{
    double tmp;

    for (int i = 0; i < trackLength; i++) {
        if (track[i][0] > track[i][2]) {
            tmp = track[i][0];
            track[i][0] = track[i][2];
            track[i][2] = tmp;
        }
        if (track[i][1] > track[i][3]) {
            tmp = track[i][1];
            track[i][1] = track[i][3];
            track[i][3] = tmp;
        }
    }
}

int Board::packageSpace(const Element &element1, const Element &element2)
{
    int dx = fabs(element2.centerX - element1.centerX);
    int dy = fabs(element2.centerY - element1.centerY);
    int w1 = fabs(element1.border.rightX - element1.border.leftX);
    int h1 = fabs(element1.border.bottomY - element1.border.topY);
    int w2 = fabs(element2.border.rightX - element2.border.leftX);
    int h2 = fabs(element2.border.bottomY - element2.border.topY);
    int sx = dx - 0.5 * (w1 + w2);
    int sy = dy - 0.5 * (h1 + h2);

    if (sx < 0)
        sx = 0;
    if (sy < 0)
        sy = 0;

    int space = sqrt(sx * sx + sy * sy);

    return space;
}

int Board::padSpace(const Element &element1, const Element &element2)
{
    int dx, dy;
    int w, h;
    int sx, sy;
    int space;
    int minSpace = 1000000;

    for (auto &e1 : element1.pads)
        for (auto &e2 : element2.pads) {
            dx = abs(e2.x - e1.x);
            dy = abs(e2.y - e1.y);
            w = 0.5 * (e1.width + e2.width);
            h = 0.5 * (e1.height + e2.height);
            sx = dx - w;
            sy = dy - h;
            if (sx < 0)
                sx = 0;
            if (sy < 0)
                sy = 0;
            space = sqrt(sx * sx + sy * sy);
            if (space < minSpace)
                minSpace = space;
       }

    return minSpace;
}

void Board::place()
{

}

void Board::placeElement()
{

}

void Board::placeElements()
{
    const int dx = 2000;
    const int dy = 8000;
    int n = 0;
    int x = dx;
    int y = 3 * dy;

    for (auto &g : groups) {
        for (auto j = g.begin(); j != g.end(); ++j) {
            if (j != g.begin())
                moveElement(*j, x, y);
            x += dx;
        }
        x += 4 * dx;
        if (n && !(n % 3)) {
            x = dx;
            y += dy;
        }
        n++;
    }
}

void Board::placeGroup(const Group &group, int refX, int refY)
{
    const int delta = 100;
    const int space = 1000;
    const int steps = 400;
    int j, k;
    int length;
    int minLength;
    int n = 0;
    int x, y;
    int xMin, yMin;

    for (auto &g : group) {
        j = g;
        if (n == 1)
            moveElement(j, refX, refY);
        if (n > 1) {
            minLength = steps * delta;
            for (int n1 = 0; n1 < steps; n1++)
                for (int n2 = 0; n2 < steps; n2++) {
                    x = refX + (n1 - 0.5 * steps) * delta;
                    y = refY + (n2 - 0.5 * steps) * delta;
                    moveElement(j, x, y);
                    if (packageSpace(elements[j], elements[k]) < space ||
                        padSpace(elements[j], elements[k]) < space)
                        continue;
                    length = netLength(elements[j], elements[k]);
                    if (length < minLength) {
                        minLength = length;
                        xMin = x;
                        yMin = y;
                    }
                }
            moveElement(j, xMin, yMin);
        }
        k = j;
        n++;
    }
}

// table cell format: cell type (8 high bits), element number (8 bits),
// pad number (8 bits), net number (8 low bits)
void Board::placePadsToTable()
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    const int startRow = 5;
    const int startColumn = 5;
    int colBusy;
    int groupNumber = 0;
    int nMax;
    int n, n2, n3;
    int row, col;
    int rowBusy;
    int tmpCol;
    int tmpRow;

    row = startRow;
    col = startColumn;
    for (auto g : groups) {
        tmpRow = row;
        tmpCol = col;
        do {
            rowBusy = 0;
            colBusy = 0;
            nMax = 1;
            row = tmpRow;
            col = tmpCol;
            for (auto i = g.begin(); i != g.end(); ++i) {
                if (i == g.begin())
                    continue;
                if (elements[*i].pads.size() == 2) {
                    if (table[row-1][col-1] || table[row-1][col] || table[row-1][col+1])
                        rowBusy = 1;
                    if (table[row][col-1] || table[row][col] || table[row][col+1] ||
                        table[row+1][col-1] || table[row+1][col] || table[row+1][col+1])
                        colBusy = 1;
                    col += 2;
                    continue;
                }
                if (table[row-1][col-1] || table[row-1][col] || table[row-1][col+1] ||
                    table[row-1][col+2] || table[row-1][col+3])
                    rowBusy = 1;
                n2 = elements[*i].pads.size();
                n3 = n2 / 2;
                if (n3 > nMax)
                    nMax = n3;
                for (n = 0; n < n3; n++)
                    if (table[row+n][col-1] || table[row+n][col] || table[row+n][col+1])
                        colBusy = 1;
                for (n = n3; n < n2; n++)
                    if (table[row-n-1+n2][col+1] || table[row-n-1+n2][col+2] ||
                        table[row-n-1+n2][col+3])
                        colBusy = 1;
                col += 4;
            }
            if (rowBusy || colBusy)
                tmpCol++;
        } while ((rowBusy || colBusy) && (row + nMax) < (rows - 5) && col < (columns - 5));

        row = tmpRow;
        col = tmpCol;
        for (auto i = g.begin(); i != g.end(); ++i) {
            if (i == g.begin())
                continue;
            if (elements[*i].pads.size() == 2) {
                table[row][col] = PAD << 24 | (*i) << 16 | 0 << 8 | elements[*i].pads[0].net;
                table[row+1][col] = PAD << 24 | (*i) << 16 | 1 << 8 | elements[*i].pads[1].net;
                col += 2;
                continue;
            }
            n2 = elements[*i].pads.size();
            n3 = n2 / 2;
            for (n = 0; n < n3; n++)
                table[row+n][col] = PAD << 24 | (*i) << 16 | n << 8 | elements[*i].pads[n].net;
            for (n = n3; n < n2; n++)
                table[row-n-1+n2][col+2] = PAD << 24 | (*i) << 16 | n << 8 | elements[*i].pads[n].net;
            col += 4;
        }
        if (groupNumber && !(groupNumber % 3)) {
            row += 3;
            col = startColumn;
        }
        groupNumber++;
    }
}

void Board::reduceTrack(double track[][4], int &trackLength)
{
    int n;

    for (int n = 0; n < 3; n++) {
        for (int i = 0; i < trackLength; i++) {
            if (track[i][0] < -0.1)
                continue;
            for (int j = i + 1; j < trackLength; j++) {
                if (track[j][0] < -0.1)
                    continue;
                if (joinLines2(track[i], track[j]))
                    track[j][0] = -1;
            }
        }
    }

    for (int i = 0; i < trackLength; i++) {
        if (track[i][0] > -0.1)
            continue;
        for (int j = i + 1; j < trackLength; j++) {
            if (track[j][0] < -0.1)
                continue;
            for (int k = 0; k < 4; k++)
                track[i][k] = track[j][k];
            track[j][0] = -1;
            break;
        }
    }

    n = trackLength;

    for (int i = 0; i < trackLength; i++)
        if (track[i][0] < -0.1) {
            n = i;
            break;
        }

    trackLength = n;
}

// Every end of line must connect with other line or pad
void Board::removeUnconnectedLines(double track[][4], int &trackLength,
                                   int *netPadsRow, int *netPadsCol, int netPadsLength)
{
    bool delta;
    double length;
    double line[4];
    double minLength;
    double minLength2;
    double minX, minY;
    double minX2, minY2;
    double maxLength;
    double maxX, maxY;
    double x, y;
    double x1, y1, x2, y2;
    int n, n1, n2;
    int number;
    int row, col;

    n = 0;
    delta = true;
    number = trackLength;

    while (delta && number && n < trackLength) {
        n++;
        delta = false;
        for (int i = 0; i < trackLength; i++) {
            if (track[i][0] < 0)
                continue;
            n1 = 0;
            n2 = 0;
            getLineCoordinates(track[i], x1, y1, x2, y2);
            for (int j = 0; j < trackLength; j++) {
                if (i == j || track[j][0] < 0)
                    continue;
                if (crossPoint2(track[j], x1, y1))
                    n1++;
                if (crossPoint2(track[j], x2, y2))
                    n2++;
            }
            for (int j = 0; j < netPadsLength; j++) {
                row = netPadsRow[j];
                col = netPadsCol[j];
                if ((fabs(floor(y1) - row) < minValue) &&
                    (fabs(floor(x1) - col) < minValue))
                    n1++;
                if ((fabs(floor(y2) - row) < minValue) &&
                    (fabs(floor(x2) - col) < minValue))
                    n2++;
            }
            if (n1 && n2)
                continue;
            delta = true;
            if (!n1 && !n2) {
                minLength = maxStep;
                minLength2 = maxStep;
                minX = 0;
                minY = 0;
                minX2 = 0;
                minY2 = 0;
                for (int j = 0; j < trackLength; j++) {
                    if (i == j || track[j][0] < 0)
                        continue;
                    if (crossLines2(track[i], track[j], x, y)) {
                        length = fabs(x1 - x) + fabs(y1 - y);                        
                        if (length > minValue && length < minLength) {
                            minLength = length;
                            minX = x;
                            minY = y;
                        }
                        length = fabs(x2 - x) + fabs(y2 - y);
                        if (length > minValue && length < minLength2) {
                            minLength2 = length;
                            minX2 = x;
                            minY2 = y;
                        }
                    }
                }
                length = fabs(minX - minX2) + fabs(minY - minY2);
                if (length > minValue && minX > minValue && minX2 > minValue &&
                    minY > minValue && minY2 > minValue) {
                    track[i][0] = minX;
                    track[i][1] = minY;
                    track[i][2] = minX2;
                    track[i][3] = minY2;
                }
                else
                    track[i][0] = -1;
            }
            if (n1) {
                maxLength = 0;
                for (int j = 0; j < trackLength; j++) {
                    if (i == j || track[j][0] < 0)
                        continue;
                    if (crossLines2(track[i], track[j], x, y)) {
                        length = fabs(x1 - x) + fabs(y1 - y);
                        if (length < minValue)
                            continue;
                        if (length > maxLength) {
                            maxLength = length;
                            maxX = x;
                            maxY = y;
                        }
                    }
                }
                for (int j = 0; j < netPadsLength; j++) {
                    row = netPadsRow[j];
                    col = netPadsCol[j];
                    if (fabs(track[i][0] - track[i][2]) < minValue) {
                        line[0] = col;
                        line[1] = row + 0.5;
                        line[2] = col + 1;
                        line[3] = row + 0.5;
                    }
                    else {
                        line[0] = col + 0.5;
                        line[1] = row;
                        line[2] = col + 0.5;
                        line[3] = row + 1;
                    }
                    if (crossLines2(track[i], line, x, y)) {
                        length = fabs(x1 - x) + fabs(y1 - y);
                        if (length < minValue)
                            continue;
                        if (length > maxLength) {
                            maxLength = length;
                            maxX = x;
                            maxY = y;
                        }
                    }
                }
                if (maxLength > minValue) {
                    track[i][2] = maxX;
                    track[i][3] = maxY;
                }
                else
                    track[i][0] = -1;
            }
            if (n2) {
                maxLength = 0;
                for (int j = 0; j < trackLength; j++) {
                    if (i == j || track[j][0] < 0)
                        continue;
                    if (crossLines2(track[i], track[j], x, y)) {
                        length = fabs(x2 - x) + fabs(y2 - y);
                        if (length < minValue)
                            continue;
                        if (length > maxLength) {
                            maxLength = length;
                            maxX = x;
                            maxY = y;
                        }
                    }
                }
                for (int j = 0; j < netPadsLength; j++) {
                    row = netPadsRow[j];
                    col = netPadsCol[j];
                    if (fabs(track[i][0] - track[i][2]) < minValue) {
                        line[0] = col;
                        line[1] = row + 0.5;
                        line[2] = col + 1;
                        line[3] = row + 0.5;
                    }
                    else {
                        line[0] = col + 0.5;
                        line[1] = row;
                        line[2] = col + 0.5;
                        line[3] = row + 1;
                    }
                    if (crossLines2(track[i], line, x, y)) {
                        length = fabs(x2 - x) + fabs(y2 - y);
                        if (length < minValue)
                            continue;
                        if (length > maxLength) {
                            maxLength = length;
                            maxX = x;
                            maxY = y;
                        }
                    }
                }
                if (maxLength > minValue) {
                    track[i][0] = maxX;
                    track[i][1] = maxY;
                }
                else
                    track[i][0] = -1;
            }
        }
        number = 0;
        for (int i = 0; i < trackLength; i++)
            if (track[i][0] > 0)
                number++;
    }

    for (int i = 0; i < trackLength; i++) {
        if (track[i][0] > 0)
            continue;
        for (int j = i + 1; j < trackLength; j++) {
            if (track[j][0] > 0) {
                for (int k = 0; k < 4; k++)
                    track[i][k] = track[j][k];
                track[j][0] = -1;
                break;
            }
        }
    }

    number = 0;
    for (int i = 0; i < trackLength; i++)
        if (track[i][0] > 0)
            number++;

    trackLength = number;
}

void Board::route()
{
/*
    for (ElementIt i = elements.begin(); i != elements.end(); ++i)
        (*i).second.pads[j].net;

    for (ViaIt i = vias.begin(); i != vias.end(); ++i)
        (*i).via.net;

    track.addSegment(int x1, int y1, int x2, int y2);
    track.clear();
    track.deleteSegment(int x1, int y1, int x2, int y2);
    track.draw(QPainter &painter);
    track.improve();
    track.improveCurve();
    track.improveSegment();
*/
}

void Board::routeTracks()
{
    const int dx = 8000;
    const int dy = 10000;
    int n = 0;
    int x = dx;
    int y = dy;

    for (auto &g : groups) {
        placeGroup(g, x, y);
        x += 2 * dx;
        if (n && !(n % 3)) {
            x = dx;
            y += dy;
        }
        n++;
    }
}

void Board::setPadSteps(int padSteps[][maxPad], int netPad, int netPadsLength,
                        int *netPadsRow, int *netPadsCol)
{
    int n;
    int row, col;

    for (int i = 0; i < netPadsLength; i++) {
        row = netPadsRow[i];
        col = netPadsCol[i];
        padSteps[netPad][i] = 0;
        if (i != netPad) {
            padSteps[netPad][i] = maxStep;
            if (n == (stepTable[row][col] >> 8) & 0xff)
                padSteps[netPad][i] = n;
        }
    }
}

void Board::setRouteBorder()
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    const int maxWaveLength = 512;
    const int deltaR8[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int deltaC8[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dr, dc;
    int n, n2;
    int row, col;
    int step;
    int waveLength;
    int newWaveLength;
    int waveRow[maxWaveLength];
    int waveCol[maxWaveLength];

    for (row = minRow-1; row <= maxRow+1; row++) {
        table[row][minColumn-1] = BORDER << 24;
        table[row][maxColumn+1] = BORDER << 24;
    }
    for (col = minColumn-1; col <= maxColumn+1; col++) {
        table[minRow-1][col] = BORDER << 24;
        table[maxRow+1][col] = BORDER << 24;
    }
    // 8 cells: type 0 or type 4
    step = 0;
    while (step < 0.5 * (maxRow - minRow)) {
        waveLength = 0;
        newWaveLength = 0;
        for (row = minRow+step; row <= maxRow-step; row++) {
            waveRow[waveLength] = row;
            waveCol[waveLength] = minColumn+step;
            waveLength++;
        }
        for (row = minRow+step; row <= maxRow-step; row++) {
            waveRow[waveLength] = row;
            waveCol[waveLength] = maxColumn-step;
            waveLength++;
        }
        for (col = minColumn+step; col <= maxColumn-step; col++) {
            waveRow[waveLength] = minRow+step;
            waveCol[waveLength] = col;
            waveLength++;
        }
        for (col = minColumn+step; col <= maxColumn-step; col++) {
            waveRow[waveLength] = maxRow-step;
            waveCol[waveLength] = col;
            waveLength++;
        }
        for (int i = 0; i < waveLength; i++) {
            row = waveRow[i];
            col = waveCol[i];
            if (table[row][col])
                continue;
            n = 0;
            for (int j = 0; j < 8; j++) {
                dr = deltaR8[j];
                dc = deltaC8[j];
                n2 = table[row+dr][col+dc] >> 24;
                if (n2 != EMPTY && n2 != BORDER) {
                    n = 1;
                    break;
                }
            }
            if (!n) {
                table[row][col] = BORDER << 24;
                newWaveLength++;
            }
        }
        if (!newWaveLength)
            break;
        step++;
    }
}

// Set track for current net
void Board::setTrack(double track[][4], int &trackLength,
                     int netPadsLength, int *netPadsRow, int *netPadsCol)
{
    enum {EMPTY};

    int netTrackLength;
    int row, col;
    int row2, col2;
    int netTrackRow[maxStep];
    int netTrackCol[maxStep];

    netTrackLength = 0;
    for (int i = minRow; i <= maxRow; i++)
        for (int j = minColumn; j <= maxColumn; j++)
            if (table[i][j] >> 24 == EMPTY) {
                netTrackRow[netTrackLength] = i;
                netTrackCol[netTrackLength] = j;
                netTrackLength++;
            }

    // Convert track cells to lines
    trackLength = 0;
    for (int i = 0; i < netPadsLength; i++) {
        for (int j = 0; j < netTrackLength; j++) {
            row = netPadsRow[i];
            col = netPadsCol[i];
            row2 = netTrackRow[j];
            col2 = netTrackCol[j];
            if ((row == row2 && fabs(col - col2) < 1.1) ||
                (col == col2 && fabs(row - row2) < 1.1))
                addLineToTrack(track, trackLength, col + 0.5,
                               row + 0.5, col2 + 0.5, row2 + 0.5);
        }
    }

    for (int i = 0; i < netTrackLength; i++) {
        for (int j = i + 1; j < netTrackLength; j++) {
            row = netTrackRow[i];
            col = netTrackCol[i];
            row2 = netTrackRow[j];
            col2 = netTrackCol[j];
            if ((row == row2 && fabs(col - col2) < 1.1) ||
                (col == col2 && fabs(row - row2) < 1.1))
                addLineToTrack(track, trackLength, col + 0.5,
                               row + 0.5, col2 + 0.5, row2 + 0.5);
        }
    }
}

void Board::setTurnSteps(int padSteps[][maxPad], int padTurns[][maxPad], int netPad,
                         int netPadsLength, int *netPadsRow, int *netPadsCol)
{
    int n;
    int row, col;

    for (int i = 0; i < netPadsLength; i++) {
        row = netPadsRow[i];
        col = netPadsCol[i];
        padSteps[netPad][i] = 0;
        padTurns[netPad][i] = 0;
        if (i != netPad) {
            padSteps[netPad][i] = maxStep;
            padTurns[netPad][i] = maxTurn;
            if (n == (stepTable[row][col] >> 8) & 0xff)
                padSteps[netPad][i] = n;
            if (n == (turnTable[row][col] >> 8) & 0xff)
                padTurns[netPad][i] = n;
        }
    }
}

void Board::sortLineIndex()
{

}

bool Board::step(int row, int col, int direction)
{/*
    table[rows][columns];
    lineX1[maxLines];           // x1 sorted from 0 to max
    lineY1[maxLines];           // trackLines is number of lines
    lineX2[maxLines];
    lineY2[maxLines];
    tracks[maxNet][maxLine][4]; // x1, y1, x2, y2
*/
    return true;
}

// table cell format: cell type (8 high bits), element number (8 bits),
// pad number (8 bits), net number (8 low bits)
int Board::tableRoute()
{
    enum CellType {EMPTY, PAD, TRACK, TRACK_VARIANT, BORDER, BUSY};

    const int combinations = 16;    
    const int maxExtension = maxStep / 4;    
    const int maxWaveLength = 512;    
    const int deltaR4[] = {-1, 1, 0, 0};    // up, down, right, left
    const int deltaC4[] = {0, 0, 1, -1};
    int dr, dc;
    int empty;
    int empty2;
    int equalCombination[combinations];
    int error = 0;
    int extension;
    int first, last;
    int minTurnSum;
    int n, n2, n3;
    int netPadsLength;
    int netTrackLength;
    int number;
    int number2;
    int padCounter;
    int r, c;
    int route;
    int row, col;
    int row2, col2;
    int step;    
    int turn;
    int turnSum;
    int waveLength;
    int waveLength2;
    int newWaveLength;
    int newWaveLength2;
    int waveRow[maxWaveLength];
    int waveCol[maxWaveLength];
    int newWaveRow[maxWaveLength];
    int newWaveCol[maxWaveLength];
    int waveRow2[maxWaveLength];
    int waveCol2[maxWaveLength];
    int newWaveRow2[maxWaveLength];
    int newWaveCol2[maxWaveLength];
    int netPadsRow[maxPad];
    int netPadsCol[maxPad];
    int netTrackRow[maxStep];
    int netTrackCol[maxStep];
    int padSteps[maxPad][maxPad];   // steps between [first pad][second pad]
    int padSteps2[maxPad][maxPad];
    int padSteps3[maxPad][maxPad];
    int padTurns[maxPad][maxPad];   // turns between [first pad][second pad]
    int padTurns2[maxPad][maxPad];
    int tmpSteps[combinations][maxPad][maxPad];
    int tmpTurns[combinations][maxPad][maxPad];    
    int tmpTable[rows][columns];   
    double x1, y1, x2, y2;
    double x3, y3;

    trackLines = 0;
    hLines = 0;
    vLines = 0;

    int maxStep3 = maxStep;

    std::fill_n(table[0], rows * columns, 0);
    std::fill_n(tracks[0][0], 4 * maxNet * maxLine, -1);

    std::fill_n(maxStep2, maxNet, maxStep3);

    placePadsToTable();

    findTableBorder();

    movePadsToTableCenter();

    setRouteBorder();

    //maxStep2[7] = 10;

    // Route tracks
    for (netNumber = 0; netNumber < nets.size(); netNumber++) {   // netNumber < nets.size()
        netPadsLength = getPadsOfNet(netPadsRow, netPadsCol);

        // Try to reduce path cells, increasing path length step by step
        /*first = 1;
        last = 0;
        extension = 0;
        while (extension < 1) { // extension < maxExtension
        // Try to reduce path cells
        r = minRow;
        while (r <= minRow) {   // r <= maxRow
            c = minColumn;
            while (c <= minColumn) {    // c <= maxColumn

                // If current cell is empty, make it busy
                if (!first && !last)
                    if (table[r][c] != EMPTY) {
                        c++;
                        continue;
                    }
                    else
                        table[r][c] = BUSY << 24;*/

                // Init step wave route for every pad of net
                for (int netPad = 0; netPad < 1; netPad++) {    // netPad < netPadsLength
                    waveRow[0] = netPadsRow[netPad];
                    waveCol[0] = netPadsCol[netPad];
                    waveLength = 1;

                    std::copy(table[0], table[0] + rows * columns, stepTable[0]);

                    std::fill_n(trackX[0], rows * columns, 0);
                    std::fill_n(trackY[0], rows * columns, 0);

                    std::fill_n(tmpTrack[0], maxLines * 4, 0);
                    tmpTrackLength = 0;

                    row = waveRow[0];
                    col = waveCol[0];
                    trackX[row][col] = col + 0.5;
                    trackY[row][col] = row + 0.5;

                    // Set pad cells empty for wave route
                    for (int i = 0; i < netPadsLength; i++)
                        if (i != netPad) {
                            row = netPadsRow[i];
                            col = netPadsCol[i];
                            stepTable[row][col] = EMPTY;
                        }

                    // Step wave route
                    padCounter = 1;
                    route = 1;
                    step = 0;
                    while (step < maxStep2[netNumber]) { // step < maxStep
                        step++;
                        newWaveLength = 0;
                        for (int i = 0; i < waveLength; i++) {
                            row = waveRow[i];
                            col = waveCol[i];                            
                            for (int j = 0; j < 4; j++) {
                                dr = deltaR4[j];
                                dc = deltaC4[j];
                                if (!stepTable[row+dr][col+dc] && newWaveLength < 256) {
                                    if (!nextCellPoint(row, col, j))
                                        continue;
                                    stepTable[row+dr][col+dc] = TRACK_VARIANT << 24 | step << 8 | netNumber;
                                    newWaveRow[newWaveLength] = row + dr;
                                    newWaveCol[newWaveLength] = col + dc;
                                    newWaveLength++;
                                    for (n = 0; n < netPadsLength; n++)
                                        if (row + dr == netPadsRow[n] && col + dc == netPadsCol[n])
                                            padCounter++;
                                    if (padCounter == netPadsLength) {
                                        route = 0;
                                        break;
                                    }
                                }
                            }
                            if (!route)
                                break;
                        }
                        if (!newWaveLength || !route)
                            break;
                        waveLength = newWaveLength;
                        for (int i = 0; i < waveLength; i++) {
                            waveRow[i] = newWaveRow[i];
                            waveCol[i] = newWaveCol[i];
                        }
                    }
/*
                    if (first)
                        setPadSteps(padSteps, netPad, netPadsLength,
                                    netPadsRow, netPadsCol);
                    if (!first && !last)
                        setPadSteps(padSteps2, netPad, netPadsLength,
                                    netPadsRow, netPadsCol);
                    if (last) {
                        setPadSteps(padSteps3, netPad, netPadsLength,
                                    netPadsRow, netPadsCol);
                        if (netPad == netPadsLength - 1) {
                            c = maxColumn + 1;
                            r = maxRow + 1;
                            extension = maxExtension;
                            break;
                        }
                    }*/
                }
/*
                if (!first && !last)
                    // Check current cell
                    for (int i = 0; i < netPadsLength; i++)
                        for (int j = 0; j < netPadsLength; j++)
                            if (i != j && padSteps2[i][j] > padSteps[i][j] + extension) {
                                table[r][c] = EMPTY;
                                break;
                            }

                if (!first)
                    c++;                
                first = 0;
            }
            r++;
        }
        extension++;
        if (!last && extension > maxExtension - 2) {
           extension = maxExtension - 2;
           last = 1;
        }
        }
*/
        //std::copy(table[0], table[0] + rows * columns, tmpTable[0]);

        // Reduce tmpTrack2[tmpTrackLength2]

        // Try decrease turns and don't increase steps
        // for every pad of current net
        // Try to change EMPTY and BUSY cells, adjacent to pad
/*      first = 1;
        last = 0;
        number = 0;
        while (number < netPadsLength) {
            row = netPadsRow[number];
            col = netPadsCol[number];
            empty = 0;
            for (int i = 0; i < 4; i++) {
                dr = deltaR4[i];
                dc = deltaC4[i];
                if (table[row+dr][col+dc] >> 24 == EMPTY)
                    empty++;
            }
            if (!first && !last && (!empty || empty == 4)) {
                number++;
                if (number > netPadsLength - 1) {
                   number = netPadsLength - 1;
                   last = 1;
                }
                continue;
            }            
            number2 = 0;
            while (number2 < combinations) {
                // Set adjacent cells
                if (!first && !last) {
                    row = netPadsRow[number];
                    col = netPadsCol[number];
                    for (int i = 0; i < 4; i++) {
                        dr = deltaR4[i];
                        dc = deltaC4[i];
                        n = table[row+dr][col+dc] >> 24;
                        if (n == EMPTY || n == BUSY) {
                            table[row+dr][col+dc] = EMPTY << 24;
                            if (number2 << i & 8)   // up, right, down, left
                                table[row+dr][col+dc] = BUSY << 24;
                        }
                    }
                    empty2 = 0;
                    for (int i = 0; i < 4; i++) {
                        dr = deltaR4[i];
                        dc = deltaC4[i];
                        if (table[row+dr][col+dc] >> 24 == EMPTY)
                            empty2++;
                    }
                    equalCombination[number2] = 0;
                    if (empty != empty2) {
                        number2++;
                        continue;
                    }
                    equalCombination[number2] = 1;
                }

                // Find step number and turn number between pads of current net
                for (int netPad = 0; netPad < netPadsLength; netPad++) {
                    waveRow[0] = netPadsRow[netPad];
                    waveCol[0] = netPadsCol[netPad];
                    waveLength = 1;
                    waveRow2[0] = netPadsRow[netPad];
                    waveCol2[0] = netPadsCol[netPad];
                    waveLength2 = 1;

                    std::copy(table[0], table[0] + rows * columns, stepTable[0]);
                    std::copy(table[0], table[0] + rows * columns, turnTable[0]);

                    for (int i = 0; i < netPadsLength; i++)
                        if (i != netPad) {
                            row = netPadsRow[i];
                            col = netPadsCol[i];
                            stepTable[row][col] = EMPTY;
                            turnTable[row][col] = EMPTY;
                        }

                    // Step wave route
                    step = 0;
                    while (step < maxStep) {
                        step++;
                        newWaveLength = 0;
                        for (int i = 0; i < waveLength; i++) {
                            row = waveRow[i];
                            col = waveCol[i];
                            if (step == 1)
                                padNumber = i;
                            else
                                padNumber = stepTable[row][col] >> 16 & 0xff;
                            for (int j = 0; j < 4; j++) {
                                dr = deltaR4[j];
                                dc = deltaC4[j];
                                if (!stepTable[row+dr][col+dc] && newWaveLength < 256) {
                                    stepTable[row+dr][col+dc] = TRACK_VARIANT << 24 |
                                             padNumber << 16 | step << 8 | netNumber;
                                    newWaveRow[newWaveLength] = row + dr;
                                    newWaveCol[newWaveLength] = col + dc;
                                    newWaveLength++;
                                }
                            }
                        }
                        if (!newWaveLength)
                            break;
                        waveLength = newWaveLength;
                        std::copy(newWaveRow, newWaveRow + waveLength, waveRow);
                        std::copy(newWaveCol, newWaveCol + waveLength, waveCol);
                    }

                    // Turn wave route
                    turn = 0;
                    while (turn < maxTurn) {
                        turn++;
                        newWaveLength2 = 0;
                        for (int i = 0; i < waveLength2; i++) {
                            row = waveRow2[i];
                            col = waveCol2[i];
                            for (int j = 0; j < 4; j++) {
                                dr = deltaR4[j];
                                dc = deltaC4[j];
                                for (int k = 1; k < maxTurn; k++) {
                                    if (turnTable[row+k*dr][col+k*dc] != EMPTY)
                                        break;
                                    if (newWaveLength2 < 256) {
                                        turnTable[row+k*dr][col+k*dc] = TRACK_VARIANT << 24 |
                                                                       turn << 8 | netNumber;
                                        newWaveRow2[newWaveLength2] = row + k * dr;
                                        newWaveCol2[newWaveLength2] = col + k * dc;
                                        newWaveLength2++;
                                    }
                                }
                            }
                        }
                        if (!newWaveLength2)
                            break;
                        waveLength2 = newWaveLength2;
                        std::copy(newWaveRow2, newWaveRow2 + waveLength2, waveRow2);
                        std::copy(newWaveCol2, newWaveCol2 + waveLength2, waveCol2);
                    }

                    if (first)
                        setTurnSteps(padSteps, padTurns, netPad,
                                     netPadsLength, netPadsRow, netPadsCol);
                    if (!first && !last)
                        setTurnSteps(tmpSteps[number2], tmpTurns[number2], netPad,
                                     netPadsLength, netPadsRow, netPadsCol);
                    if (last) {
                        setTurnSteps(padSteps2, padTurns2, netPad,
                                     netPadsLength, netPadsRow, netPadsCol);
                        if (netPad == netPadsLength - 1) {
                            number = netPadsLength;
                            number2 = combinations;
                            break;
                        }
                    }
                }

                if (!first)
                    number2++;
                first = 0;
            }

            if (!first && !last) {
                // Select variant with minimum sum of turns
                n = 0;
                for (int i = 0; i < combinations; i++) {
                    if (!equalCombination[i])
                        continue;
                    minTurnSum = 1000000;
                    turnSum = 0;
                    n2 = 0;
                    for (int j = 0; j < netPadsLength; j++) {
                        for (int k = 0; k < netPadsLength; k++) {
                            if (tmpSteps[i][j][k] > padSteps[j][k]) {
                                n2 = 1;
                                break;
                            }
                            turnSum += tmpTurns[i][j][k];
                        }
                        if (n2)
                            break;
                    }
                    if (n2 || turnSum < 0 || turnSum > 10000)
                        continue;
                    if (turnSum < minTurnSum) {
                        minTurnSum = turnSum;
                        n = i;
                    }
                }
                row = netPadsRow[number];
                col = netPadsCol[number];
                for (int i = 0; i < 4; i++) {
                    dr = deltaR4[i];
                    dc = deltaC4[i];
                    n3 = table[row+dr][col+dc] >> 24;
                    if (n3 == EMPTY || n3 == BUSY) {
                        table[row+dr][col+dc] = EMPTY << 24;
                        if (n << i & 8)
                            table[row+dr][col+dc] = BUSY << 24;
                    }
                }
            }

            number++;
            if (!last && number > netPadsLength - 1) {
               number = netPadsLength - 1;
               last = 1;
            }
        }
*/
        //setTrack(tmpTrack, tmpTrackLength, netPadsLength, netPadsRow, netPadsCol);

        //if (netNumber < 7)
        reduceTrack(tmpTrack, tmpTrackLength);

        orderTrackLines(tmpTrack, tmpTrackLength);

        //if (netNumber < 7)
        removeUnconnectedLines(tmpTrack, tmpTrackLength,
                               netPadsRow, netPadsCol, netPadsLength);

        orderTrackLines(tmpTrack, tmpTrackLength);

        connectPadCenter(tmpTrack, tmpTrackLength);

        // Update track line array
        if (tmpTrackLength > maxLine)
            tmpTrackLength = maxLine;
        for (int i = 0; i < tmpTrackLength; i++)
            for (int j = 0; j < 4; j++)
                tracks[netNumber][i][j] = tmpTrack[i][j];
        trackLine[netNumber] = tmpTrackLength;

        // Clear busy cells
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < columns; j++)
                if (table[i][j] >> 24 == BUSY)
                    table[i][j] = EMPTY << 24;

        // Update track line index arrays
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < tmpTrackLength; j++) {
                n = (netNumber << 8) + j;
                lineIndex[i][trackLines+j] = n;
            }
        trackLines += tmpTrackLength;

        // Sort track line index arrays
        for (int i = 0; i < 4; i++) {
            std::sort(lineIndex[i], lineIndex[i] + trackLines,
                      [this, i] (int n1, int n2) -> bool {
                return lineCoordinate(i, n1) < lineCoordinate(i, n2); });
        }

        // Update hLine and vLine index arrays
        for (int i = 0; i < tmpTrackLength; i++) {
            getLineCoordinates(tracks[netNumber][i], x1, y1, x2, y2);
            if (fabs(y1 - y2) < minValue)   // select horizontal lines
                hLineIndex[hLines++] = (netNumber << 8) + i;
            if (fabs(x1 - x2) < minValue)   // select vertical lines
                vLineIndex[vLines++] = (netNumber << 8) + i;
        }

        // Sort hLine index array by y1
        std::sort(hLineIndex, hLineIndex + hLines,
                  [this] (int n1, int n2) -> bool {
            return lineCoordinate(1, n1) < lineCoordinate(1, n2); });

        // Sort vLine index array by x1
        std::sort(vLineIndex, vLineIndex + vLines,
                  [this] (int n1, int n2) -> bool {
            return lineCoordinate(0, n1) < lineCoordinate(0, n2); });
    }

    return error;
}

int Board::waveRoute()
{
    const int maxX = 1024;
    const int maxY = 1024;
    const int area = maxX * maxY;
    const int bufferLength = 4 * area;
    const int length = 8 * (maxX + maxY);
    const int maxStep = length;
    const int endNumber = 2 * maxStep;
    const int grid = 100;   // um
    const int deltaX[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int deltaY[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int endState;
    int net;
    int startX, startY;
    int endX, endY;
    int dx, dy;
    int dx2, dy2;
    int x, y;
    int x2, y2;
    int waveLength, newWaveLength;
    int pathLength;
    int step;
    int segmentStart;
    int *buffer = new int[bufferLength];
    int *board = buffer;
    int *number = buffer + area;
    int *waveX = buffer + 2 * area;
    int *waveY = waveX + length;
    int *newWaveX = waveX + 2 * length;
    int *newWaveY = waveX + 3 * length;
    int *pathX = waveX + 4 * length;
    int *pathY = pathX + length;
    //Segment segment;
    //Track track;

    std::fill_n(buffer, bufferLength, 0);

    // Route border
    for (int i = 0; i < maxX; i++) {
        *(board + maxX * i) = -1;
        *(board + maxX * i + maxY - 1) = -1;
    }
    for (int i = 0; i < maxY; i++) {
        *(board + i) = -1;
        *(board + (maxX - 1) * maxY + i) = -1;
    }

    for (auto e : elements) {
        for (auto ep : e.pads) {
            net = ep.net;
            x = ep.x / grid;
            y = ep.y / grid;
            *(board + maxX * x + y) = net;
        }
    }

    startX = 10;
    startY = 10;
    endX = 100;
    endY = 100;

    endState = 0;
    step = 1;
    *(number + maxX * startX + startY) = step;
    *(number + maxX * endX + endY) = endNumber;
    *waveX = startX;
    *waveY = startY;
    waveLength = 1;

    // find paths
    while (step < maxStep) {
        step++;
        newWaveLength = 0;
        for (int i = 0; i < waveLength; i++) {
            x = *(waveX + i);
            y = *(waveY + i);
            if (x < 1 || x > maxX - 3 || y < 1 || y > maxY - 3)
                continue;
            for (int j = 0; j < 8; j++) {
                x2 = x + deltaX[j];
                y2 = y + deltaY[j];
                if (!(*(number + maxX * x2 + y2))) {
                    *(number + maxX * x2 + y2) = step;
                    *(newWaveX + newWaveLength) = x2;
                    *(newWaveY + newWaveLength) = y2;
                    newWaveLength++;
                }
                else if (*(number + maxX * x2 + y2) == endNumber)
                    endState = 1;
            }
        }
        if (endState)
            break;
        waveLength = newWaveLength;
        for (int i = 0; i < waveLength; i++) {
            *(waveX + i) = *(newWaveX + i);
            *(waveY + i) = *(newWaveY + i);
        }
    }

    // end point is any point of net, adjacent with point value = step - 1
    pathX[0] = endX;
    pathY[0] = endY;
    pathLength = 1;
    for (int i = 0; i < length - 1; i++) {
        step--;
        x = pathX[i];
        y = pathY[i];
        for (int j = 0; j < 8; j++) {
            x2 = x + deltaX[j];
            y2 = y + deltaY[j];
            if (*(number + maxX * x2 + y2) == step) {
                pathX[i+1] = x2;
                pathY[i+1] = y2;
                pathLength++;
                break;
            }
        }
        if (step == 1)
            break;
    }

    pointX.resize(pathLength);
    pointY.resize(pathLength);
    for (int i = 0; i < pathLength; i++) {
        pointX[i] = pathX[i];
        pointY[i] = pathY[i];
    }

    // select maximal wide path without points of other nets inside path
    // select maximal sum of adjacent points with equal step value

    // select center path from end point to start point


    // path with minimum lines
    segmentStart = 1;
    for (int i = 0; i < pathLength - 1; i++) {
        if (segmentStart) {
            x = pathX[i];
            y = pathY[i];
            dx = pathX[i+1] - x;
            dy = pathY[i+1] - y;
            segmentStart = 0;
            continue;
        }
        else {
            dx2 = pathX[i+1] - pathX[i];
            dy2 = pathY[i+1] - pathY[i];
            if (dx != dx2 || dy != dy2) {
                segment.net = net;
                segment.x1 = x;
                segment.y1 = y;
                segment.x2 = pathX[i];
                segment.y2 = pathY[i];
                track.push_back(segment);
                segmentStart = 1;
                i--;
            }
            if (i == pathLength - 2) {
                segment.net = net;
                segment.x1 = x;
                segment.y1 = y;
                segment.x2 = pathX[i+1];
                segment.y2 = pathY[i+1];
                track.push_back(segment);
            }
        }
    }

    // add lines to replace 90 degree angle with 2 45 degree angles

    delete [] buffer;

    return track.size();
}

/*
void placer(Board &board, Place place)
{
    place.grid;
    place.packageSpace;
}

void router(Board &board, Route route)
{

}

int Board::greaterLine(int *lineIndex, int lines, int coordinate, double value)
{
    int index = int (std::upper_bound(lineIndex, lineIndex + lines,
                     value, [this, coordinate] (int n1, int n2) -> bool {
    return lineCoordinate(coordinate, n1) < lineCoordinate(coordinate, n2); }) - lineIndex);

    return index;
}

int Board::lessLine(int *lineIndex, int lines, int coordinate, double value)
{
    int index = int (std::lower_bound(lineIndex, lineIndex + lines,
                     value, [this, coordinate] (int n1, int n2) -> bool {
    return lineCoordinate(coordinate, n1) < lineCoordinate(coordinate, n2); }) - lineIndex - 1);

    return index;
}

if (netNumber == 5 && row == 10 && col == 19) {
    printf("side = %d, nearestVerticalLine = %d\n", side, n);
    printf("x = %.3f, y = %.3f\n", x, y);
    printf("Horizontal: x11 = %.3f, y11 = %.3f, x12 = %.3f\n", x11, y11, x12);
    printf("Vertical: x21 = %.3f, y21 = %.3f, y22 = %.3f\n", x21, y21, y22);
    printf("Vertical: x31 = %.3f, y31 = %.3f, y32 = %.3f\n", x31, y31, y32);
    printf("Vertical: x41 = %.3f, y41 = %.3f, y42 = %.3f\n", x41, y41, y42);
    printf("x1 = %.3f, y1 = %.3f, x2 = %.3f, y2 = %.3f\n", x1, y1, x2, y2);
    printf("x3 = %.3f, y3 = %.3f\n", x3, y3);
}
*/
