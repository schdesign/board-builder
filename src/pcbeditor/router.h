// router.h
// Copyright (C) 2018 Alexander Karpeko
// Coordinate unit: 1 micrometer

#ifndef ROUTER_H
#define ROUTER_H

#include <list>

typedef std::list<int> Group;      // number of group: group[0]
typedef std::list<Group> Groups;   // number of group >= 10000h

// Placer parameters
class Placer
{
public:
    double grid;            // grid step
    double packageSpace;    // space between packages
    int groupNumber;
    Groups groups;
};

// Route parameters
class Router
{
public:
    double minWidth;        // minimum width
    double width;           // signal wire width
    double powerWidth;      // power wire width
    double groundWidth;     // ground wire width
    double clearance;       // clearance between wires
    double powerClearance;  // clearance from power wire
    double maxClearance;    // maximum clearance between wires
};

#endif  // ROUTER_H
