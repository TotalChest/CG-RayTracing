#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>
#include <cstdint>
#include <cassert>
#include <iostream>


struct Color
{
	uint32_t R;
	uint32_t G;
	uint32_t B;

	Color(const uint32_t &r, const uint32_t &g, const uint32_t &b);
	uint32_t hex();
	Color operator*(const float k);
	Color operator+(const Color &K);
};

struct Point;

struct Vector
{
    float x;
    float y;
    float z;

    Vector();
    Vector(const float &x, const float &y, const float &z);
    float operator*(const Vector &B);
    Vector operator+(const Vector &B);
    Vector operator/(const float c);
    float norm();
    Point to_point(const float c);
    Vector operator*(const float c);
};

struct Point
{
    float x;
    float y;
    float z;

    Point();
    Point(const float &x, const float &y, const float &z);
    Vector operator-(Point &B);
    Point operator+(Point &B);
};

struct Camera
{
    Point O;
    Vector dir;
    float FOV;

    Camera(const Point &o, const Vector &d, const float &fov);
    Vector point_to_vector(int i, int j);
};


struct Material
{
    Color color; // Цвет
    int specular; // Гладкость
    float specular_index; // Уровень гладкости
    float reflective; // Отражение
    float refractive_index; // Степень прозрачности
    float refractive; // Коэф. преломления
    

    Material();
    Material(const Color &c, const int &s, const float &s_i, const float &re, const float &ra_i, const float &ra);
};


struct Object
{
	Material material;
	Object();
	Object(const Material &mat);
	virtual Vector get_normal(Point &P) = 0;
	virtual std::pair<float, float> IntersectRay(Point &O, Vector &D) = 0;
};


struct Sphere : Object
{
    Point center;
    float radius;

    Sphere();
    Sphere(const Point &c, const float &r, const Material &mat);
    Vector get_normal(Point &P);
    std::pair<float, float> IntersectRay(Point &O, Vector &D);
};


struct Plane : Object
{
    Vector normal;
    Point point;

    Plane();
    Plane(const Vector &n, const Point &p, const Material &mat);
    Vector get_normal(Point &P);
    std::pair<float, float> IntersectRay(Point &O, Vector &D);
};


struct Light
{
	size_t type;
    float intensity;
    Point position;
    Vector direction;

    Light(const size_t &t, const float &intens);
    Light(const size_t &t, const float &intens, const Point &p);
    Light(const size_t &t, const float &intens, const Vector &v);
};


#endif