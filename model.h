#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "geometry.h"

class Model {
private:
    std::vector<Point> verts;
    std::vector<Vector> faces;
public:
    bool exist;
    Material material;
    Model() {exist = false;}
    Model(const char *filename);

    int nverts() const;                          
    int nfaces() const;              

    bool ray_triangle_intersect(const int &fi, Point &orig, Vector &dir, float &tnear);

    const Point &point(int i) const;
    Point &point(int i);
    int vert(int fi, int li) const;
    void get_bbox(Point &min, Point &max);
};

#endif