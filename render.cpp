#include "properties.h"
#include "render.h"


std::pair<float, float> IntersectRaySphere(Point &O, Vector &D, Sphere &sphere)
{
    Point C(sphere.center);
    float r = sphere.radius;
    Vector OC = O - C;

    float k1 = D * D;
    float k2 = 2 * (OC * D);
    float k3 = OC * OC - r*r;

    float discriminant = k2*k2 - 4*k1*k3;
    if(discriminant < 0)
        return std::make_pair(INF, INF);

    float t1 = (-k2 + sqrt(discriminant)) / (2*k1);
    float t2 = (-k2 - sqrt(discriminant)) / (2*k1);
    return std::make_pair(t1, t2);
}


float ComputeLighting(Point &P, Vector &N, std::vector<Light> &lights)
{
    float i = 0.0;
    for(Light & l : lights)
    {
        if (l.type == 0)
            i += l.intensity;
        else
		{
			Vector L(0, 0, 0);
            if (l.type == 1)
                L = l.position - P;
            else
                L = l.direction;

            //std::cout << L.x << ' '<< L.y << ' '<< L.z << '\n';
            float n_dot_l = N * L;
            n_dot_l /= (N.norm()*L.norm());
            if (n_dot_l > 0)
                i += l.intensity * n_dot_l;
        }
    }
    return i;
}


Color TraceRay(Point &O, Vector &D, float t_min, float t_max, std::vector<Sphere> &objects, std::vector<Light> &lights)
{
    float closest_t = INF;
    bool closest_sphere = false;
    Sphere sphere(Point(0, 0, 0), 0, Color(0,0,0));
    for(Sphere &sph: objects)
    {
        std::pair<float, float> t = IntersectRaySphere(O, D, sph);
        if (t.first >= t_min and t.first <= t_max and t.first < closest_t)
        {
            closest_t = t.first;
            closest_sphere = true;
            sphere = sph;
        }
        if (t.second >= t_min and t.second <= t_max and t.second < closest_t)
        {
            closest_t = t.second;
            closest_sphere = true;
			sphere = sph;        
        }
    }

    if(!closest_sphere)
        return Color(30, 0 ,80);

    Point P = D * closest_t;
    P = O + P;
    Vector N = P - sphere.center;
    N = N / N.norm();
    return sphere.color * ComputeLighting(P, N, lights);
}


void render(std::vector<uint32_t> &image, Camera &camera, std::vector<Sphere> &objects, std::vector<Light> &lights)
{

	for(int i = (-1)*(int)HEIGHT/2; i < HEIGHT/2; ++i)
	{
    	for(int j = (-1)*(int)WIDTH/2; j < WIDTH/2; ++j)
    	{
        	Vector D = camera.point_to_vector(i, j);
        	Color color = TraceRay(camera.O, D, 1, INF, objects, lights);
        	image[(i+HEIGHT/2)*WIDTH + (j+WIDTH/2)] = color.hex();
    	}
	}

   return;
}


void build_image(std::vector<uint32_t> &image, int sceneId)
{

	switch(sceneId)
	{
		case 0:
		{

			std::vector<Sphere> objects;
		    objects.emplace_back(Point(0, 0, 20), 5, Color(0,0,255));
		    objects.emplace_back(Point(3, 0, 10), 1, Color(255,0,0));
		    objects.emplace_back(Point(-1, -4, 30), 7, Color(0,255,0));
		    objects.emplace_back(Point(-10, 10, 60), 30, Color(255,128,0));

		    std::vector<Light> lights;
		    lights.push_back(Light(1, 0.7, Point(30,30,0)));
		    lights.push_back(Light(2, 0.25, Vector(0,-30,0)));
		    lights.push_back(Light(0, 0.05, Point(30,30,0)));

		    Camera camera(Point(0,0,0), Vector(0,0,1), 60);

		    render(image, camera , objects, lights);

		    std::cout << "Scene 0" << std::endl;

		    break;
		}
		case 1:
		{
			std::cout << "Scene 1" << std::endl;

			break;
		}
		case 2:
		{
			std::cout << "Scene 2" << std::endl;

			break;
		}
		case 3:
		{
			std::cout << "Scene 3" << std::endl;

			break;
		}
		default:
		{
			std::cout << "Bad scene" << std::endl;

			break;
		}
	}

    return;
}