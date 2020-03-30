#include "model.h"


Model::Model(const char *filename) : verts(), faces() {
    exist = true;
    material = Material();
    std::ifstream in;
    in.open ((MODELS_DIR + filename).c_str(), std::ifstream::in);
    if (in.fail()) {
        std::cerr << "Failed to open model file: " << filename << std::endl;
        exit(1);
    }
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Point v;
            iss >> v.x;
            iss >> v.y;
            iss >> v.z;
            verts.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            Vector f;
            int idx, cnt=0;
            iss >> trash;
            iss >> f.x;
            f.x--;
            iss >> f.y;
            f.y--;
            iss >> f.z;
            f.z--;
            faces.push_back(f);
        }
    }
}


bool Model::ray_triangle_intersect(const int &fi, Point &orig, Vector &dir, float &tnear) {
    Vector edge1 = point(vert(fi,1)) - point(vert(fi,0));
    Vector edge2 = point(vert(fi,2)) - point(vert(fi,0));
    Vector pvec = cross(dir, edge2);
    float det = edge1 * pvec;
    if (det < 1e-5 && det > -1e-5) return false;

    Vector tvec = orig - point(vert(fi,0));
    float u = tvec * pvec;
    if (u < 0 || u > det) return false;

    Vector qvec = cross(tvec, edge1);
    float v = dir * qvec;
    if (v < 0 || u + v > det) return false;

    tnear = edge2 * qvec * (1./det);
    return tnear>1e-5;
}


int Model::nverts() const {
    return (int)verts.size();
}

int Model::nfaces() const {
    return (int)faces.size();
}

void Model::get_bbox(Point &min, Point &max) {
    min = max = verts[0];
    for (int i=1; i<(int)verts.size(); ++i) {
        min.x = std::min(min.x, verts[i].x);
        max.x = std::max(max.x, verts[i].x);
        min.y = std::min(min.y, verts[i].y);
        max.y = std::max(max.y, verts[i].y);
        min.z = std::min(min.z, verts[i].z);
        max.z = std::max(max.z, verts[i].z);
    }
}

const Point &Model::point(int i) const {
    assert(i>=0 && i<nverts());
    return verts[i];
}

Point &Model::point(int i) {
    assert(i>=0 && i<nverts());
    return verts[i];
}

int Model::vert(int fi, int li) const {
    assert(fi>=0 && fi<nfaces() && li>=0 && li<3);
    if(li == 0)
        return (int)faces[fi].x;
    if(li == 1)
        return (int)faces[fi].y;
    if(li == 2)
        return (int)faces[fi].z;
}