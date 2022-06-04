#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef POINTS_AND_VECTORS
#define POINTS_AND_VECTORS


typedef struct point{
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float tx;
    float ty;
}*Point;

typedef struct simplePoint{
    float x;
    float y;
    float z;
}*SimplePoint;

Point new_point(float x, float y, float z, float nx, float ny, float nz, float tx, float ty);

SimplePoint new_simplePoint(float x, float y, float z);

void write_point(float x, float y, float z, float nx, float ny, float nz, float tx, float ty, std::ofstream& file);

SimplePoint getVector(Point v1, Point v2);

SimplePoint getVector(SimplePoint v1, SimplePoint v2);

Point joinPointVector(SimplePoint point, SimplePoint vector, float tx, float ty);

void normalizeVector(Point& vec);

void normalizeVector(SimplePoint& vec);

SimplePoint normal_at_point_torus(float x, float y, float z, float radius);

void write_point(Point p, std::ofstream& file);

SimplePoint normal_cone(float x, float y, float z,float height, float radius, float declive);

SimplePoint bernsteins_polinomials(float t, SimplePoint p0, SimplePoint p1, SimplePoint p2, SimplePoint p3);

SimplePoint *get_patch_points(SimplePoint points[], int patch[], int N);

#endif