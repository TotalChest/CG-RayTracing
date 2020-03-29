#include "geometry.h"

extern const int HEIGHT;
extern const int WIDTH;
extern const float PI;
extern const float INF;
extern const float EPSILON;


Color::Color() {}
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



Vector::Vector(): x(0), y(0), z(0) {}   
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



Point::Point(): x(0), y(0), z(0) {}    
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



Material::Material(): color(Color(0,0,0)), specular(0), specular_index(0), reflective(0), refractive_index(0), refractive(0) {}
Material::Material(const Color &c, const int &s, const float &s_i, const float &re, const float &ra_i, const float &ra): color(c), specular(s), specular_index(s_i), reflective(re), refractive_index(ra_i), refractive(ra) {}



Object::Object(): material() {}
Object::Object(const Material &mat): material(mat) {}



Sphere::Sphere(): center(Point(0,0,0)), radius(0), Object() {}
Sphere::Sphere(const Point &c, const float &r, const Material &mat): center(c), radius(r), Object(mat) {}
Vector Sphere::get_normal(Point &P)
{
    Vector N = P - center;
    N = N / N.norm();
    return N;
}
std::pair<float, float> Sphere::IntersectRay(Point &O, Vector &D)
{
    float r = radius;
    Vector OC = O - center;

    float k1 = D * D;
    float k2 = 2.f * (OC * D);
    float k3 = OC * OC - r*r;

    float discriminant = k2*k2 - 4.f*k1*k3;
    if(discriminant < 0.f)
        return std::make_pair(INF, INF);

    float t1 = (-k2 + sqrt(discriminant)) / (2.f*k1);
    float t2 = (-k2 - sqrt(discriminant)) / (2.f*k1);
    return std::make_pair(t1, t2);
}



Plane::Plane(): normal(Vector(0,0,0)), point(Point(0, 0, 0)), Object() {}
Plane::Plane(const Vector &n, const Point &p, const Material &mat): normal(n), point(p), Object(mat) {}
Vector Plane::get_normal(Point &P) { return normal;}
std::pair<float, float> Plane::IntersectRay(Point &O, Vector &D)
{
    if (fabs(D * normal / normal.norm()) > 1e-5)
    {
        float plane_dist = (-1.0)*((O - point) * normal / normal.norm()) / (D * normal / normal.norm());
        if (plane_dist > 0)
            return std::make_pair(plane_dist, INF); 
    }
    return std::make_pair(INF, INF);
}



Light::Light(const size_t &t, const float &intens) : type(t), intensity(intens), position(Point(0,0,0)), direction(Vector(0,0,0)) {}

Light::Light(const size_t &t, const float &intens, const Point &p) : type(t), intensity(intens), position(p), direction(Vector(0,0,0)) {}

Light::Light(const size_t &t, const float &intens, const Vector &v) : type(t), intensity(intens), direction(v), position(Point(0,0,0)) {}



Vector cross(Vector v1, Vector v2) {
    return Vector(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

