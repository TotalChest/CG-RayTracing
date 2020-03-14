#ifndef RENDER_H
#define RENDER_H

#include "properties.h"


struct Color
{
	uint32_t R;
	uint32_t G;
	uint32_t B;

	Color(const uint32_t &r, const uint32_t &g, const uint32_t &b): R(r), G(g), B(b) {}

	uint32_t hex()
	{
		uint32_t C = 0;
		C = C | B;
		C = C | (G << 8); 
		C = C | (R << 16);
		return C;
	}

	Color operator*(float k)
	{
		return Color(R*k>255?255:R*k, G*k>255?255:G*k, B*k>255?255:B*k);
	}
};


struct Point;


struct Vector
{
    float x;
    float y;
    float z;

    Vector(const float &x, const float &y, const float &z): x(x), y(y), z(z) {}

    float operator*(const Vector &B)
    {
        return x * B.x + y * B.y + z * B.z;
    }

    Vector operator/(const float c)
	{
		return Vector(x / c, y / c, z / c);
	}

    float norm()
    {
        return sqrtf(x*x + y*y + z*z);
    }

   Point operator*(const float c);

};


struct Point
{
    float x;
    float y;
    float z;

    Point(const float &x, const float &y, const float &z): x(x), y(y), z(z) {}

    Vector operator-(Point &B)
    {
        return Vector(x - B.x, y - B.y, z - B.z);
    }

    Point operator+(Point &B)
    {
        return Point(x + B.x, y + B.y, z + B.z);
    }
};


Point Vector::operator*(float c)
{
	return Point(x*c, y*c, z*c);
}

struct Camera
{
    Point O;
    Vector dir;
    float FOV;

    Camera(const Point &o, const Vector &d, const float &fov): O(o), dir(d), FOV(fov) {}

    Vector point_to_vector(int i, int j)
    {
    	return Vector(j*2*tan(FOV*PI/180/2)/WIDTH, i*2*tan(FOV*PI/180/2)/HEIGHT, 1);
	}
};


struct Sphere
{
    Point center;
    float radius;
    Color color;

    Sphere(const Point &c, const float &r, const Color &col) : center(c), radius(r), color(col) {}

};


struct Light
{
	size_t type;
    float intensity;
    Point position;
    Vector direction;

    Light(const size_t &t, const float &intens) : type(t), intensity(intens), position(Point(0,0,0)), direction(Vector(0,0,0)) {}

    Light(const size_t &t, const float &intens, const Point &p) : type(t), intensity(intens), position(p), direction(Vector(0,0,0)) {}

    Light(const size_t &t, const float &intens, const Vector &v) : type(t), intensity(intens), direction(v), position(Point(0,0,0)) {}

};


void build_image(std::vector<uint32_t> &, int);

void render(std::vector<uint32_t> &, Camera &, Sphere &);


#endif