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

struct Sphere
{
    Point center; // Центр
    float radius; // Радиус
    Color color; // Цвет
    int specular; // Гладкость
    float reflective; // Отражение

    Sphere();
    Sphere(const Point &c, const float &r, const int &s, const float &refl,const Color &col);
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