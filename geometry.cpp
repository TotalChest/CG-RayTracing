#include "geometry.h"

extern const int HEIGHT;
extern const int WIDTH;
extern const float PI;

Color::Color(const uint32_t &r, const uint32_t &g, const uint32_t &b): R(r), G(g), B(b) {}
uint32_t Color::hex() {
	uint32_t C = 0;
	C = C | R;
	C = C | (G << 8); 
	C = C | (B << 16);
	return C;
}
Color Color::operator*(const float k){
	return Color(R*k>255?255:R*k, G*k>255?255:G*k, B*k>255?255:B*k);
}
Color Color::operator+(const Color &K){
	return Color((R+K.R)>255?255:(R+K.R), (G+K.G)>255?255:(G+K.G), (B+K.B)>255?255:(B+K.B));
}



Vector::Vector(const float &x, const float &y, const float &z): x(x), y(y), z(z) {}
float Vector::operator*(const Vector &B) {
    return x * B.x + y * B.y + z * B.z;
}
Vector Vector::operator+(const Vector &B) {
    return Vector(x + B.x, y + B.y, z + B.z);
}
Vector Vector::operator/(const float c){
	return Vector(x / c, y / c, z / c);
}
float Vector::norm() {
    return sqrtf(x*x + y*y + z*z);
}
Point Vector::to_point(float c) {
    return Point(x*c, y*c, z*c);
}
Vector Vector::operator*(const float c) {
		return Vector(x*c, y*c, z*c);
}


    
Point::Point(const float &x, const float &y, const float &z): x(x), y(y), z(z) {}
Vector Point::operator-(Point &B) {
    return Vector(x - B.x, y - B.y, z - B.z);
}
Point Point::operator+(Point &B){
    return Point(x + B.x, y + B.y, z + B.z);
}



Camera::Camera(const Point &o, const Vector &d, const float &fov): O(o), dir(d), FOV(fov) {}
Vector Camera::point_to_vector(int i, int j) {
	return Vector(j*2*tan(FOV*PI/180/2)/WIDTH, i*2*tan(FOV*PI/180/2)/WIDTH, 1);
}



Refractive::Refractive(): refract(0), index(1) {}
Refractive::Refractive(const float &r, const float &i): refract(r), index(i) {}



Material::Material(): color(Color(0,0,0)), specular(0), reflective(0), refractive() {}
Material::Material(const Color &c, const int &s, const float &re, const Refractive &ra): color(c), specular(s), reflective(re), refractive(ra) {}



Sphere::Sphere(): center(Point(0,0,0)), radius(0), material() {}
Sphere::Sphere(const Point &c, const float &r, const Material &mat): center(c), radius(r), material(mat) {}



Light::Light(const size_t &t, const float &intens) : type(t), intensity(intens), position(Point(0,0,0)), direction(Vector(0,0,0)) {}

Light::Light(const size_t &t, const float &intens, const Point &p) : type(t), intensity(intens), position(p), direction(Vector(0,0,0)) {}

Light::Light(const size_t &t, const float &intens, const Vector &v) : type(t), intensity(intens), direction(v), position(Point(0,0,0)) {}
