#include "properties.h"
#include "geometry.h"
#include "objects.h"

 
std::pair<float, float> IntersectRaySphere(Point &O, Vector &D, Sphere &sphere)
{
    float r = sphere.radius;
    Vector OC = O - sphere.center;

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


Vector ReflectRay(Vector &V, Vector &N) {
	Vector C  = N* (N * V * 2.f);
    C = C + (V * (-1.f));
    return C;
}


Vector RefractRay(Vector &V, Vector &N, float &refractive) {
    float cos = (V * N)/(V.norm() * N.norm());
    float n1 = 1, n2 = refractive;
    Vector n = N;
    if (cos < 0) {
        cos = -cos;
        std::swap(n1 , n2);
        n = N * (-1);
    }
    float A = n1 / n2;
    float k = 1 - A*A*(1 - cos*cos);
    return k < 0 ? Vector(0, 0, 0) : V*A*(-1.f) + n*(A*cos - sqrt(k));
}


bool ClosestIntersection(Point &O, Vector &D, float t_min, float t_max, float &clo_t, Sphere &sp)
{
    float closest_t = INF;
    bool closest_sphere = false;
    Sphere sphere;
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

    clo_t = closest_t;
    sp = sphere;

    return closest_sphere;
}


std::pair<float, float> ComputeLighting(Point &P, Vector &N, Vector &V, int specular, float specular_index)
{
    float d = 0.0, s = 0.0;
    for(Light & l : lights)
    {
        if (l.type == 0)
            d += l.intensity;
        else
		{
			Vector L(0, 0, 0);
			float t_max;
            if (l.type == 1)
            {
                L = l.position - P;
                t_max = 1.f;
            }
            else
            {
                L = l.direction;
                t_max = INF;
            }

           
            float closest_t;
    		Sphere sphere;
            if(ClosestIntersection(P, L, EPSILON, t_max, closest_t, sphere))
                continue;


            float k = (N * L)/(N.norm()*L.norm());
            d += l.intensity * std::max(0.f, k);


            if (specular != -1)
            {
            	Vector R = ReflectRay(L, N);
                k = (R * V)/(R.norm() * V.norm());
                if (k > 0.f)
                    s += l.intensity * specular_index * pow(k , specular);
           	}

        }
    }
    return std::make_pair(d, s);
}


Color TraceRay(Point &O, Vector &D, float t_min, float t_max, int depth)
{
    float closest_t;
    Sphere sphere;

    if(!ClosestIntersection(O, D, t_min, t_max, closest_t, sphere))
        return Back_ground;

    Point P = (D.to_point(closest_t) + O);
    Vector N = P - sphere.center;
    N = N / N.norm();
    Vector V = D * (-1.f);

    std::pair<float, float> light = ComputeLighting(P, N, V, sphere.material.specular, sphere.material.specular_index);
    Color local_color = sphere.material.color * light.first;
    
    if(depth <= 0)
    	return local_color + Color(255,255,255) * light.second;

    float h = sphere.material.refractive_index;
    float r = std::min(1 - h, sphere.material.reflective);

    local_color = local_color * (1 - h - r);

    if(r > 0)
    {
        Vector R = ReflectRay(V, N);
        local_color = local_color + TraceRay(P, R, EPSILON, INF, depth - 1) * r;
    }
    
    if(h > 0)
    {
        Vector S = RefractRay(V, N, sphere.material.refractive);
        local_color = local_color + TraceRay(P, S, EPSILON, INF, depth - 1) * h;
    }

    return local_color + Color(255,255,255) * light.second;
}



void render(std::vector<uint32_t> &image, Camera &camera)
{
	omp_set_num_threads(threads);
	#pragma omp parallel for
	for(int i = (-1)*HEIGHT/2; i < HEIGHT/2; ++i)
	{
    	for(int j = (-1)*WIDTH/2; j < WIDTH/2; ++j)
    	{
        	Vector D = camera.point_to_vector(i, j);
        	Color color = TraceRay(camera.O, D, 1, INF, RECURSION_DEPTH);
        	image[(i+HEIGHT/2)*WIDTH + (j+WIDTH/2)] = color.hex();
    	}
	}

   return;
}


bool build_image(std::vector<uint32_t> &image, int sceneId)
{

	switch(sceneId)
	{
		case 0:
		{
            Material glass(Color(200,200,200), 100, 0.7, 0.2, 0.8, 5);
            Material red(Color(200,20,0), 2, 0.1, 0.05, 0, 1);
            Material mirror(Color(100,100,100), 800, 2, 0.8, 0, 1);
            Material green(Color(40,150,30), 200, 0.2, 0.3, 0, 1);
            Material pastel(Color(215,130,80), 600, 1, 0.2, 0, 1);

		    objects.emplace_back(Point(0, 0, 17), 4, glass);
		    objects.emplace_back(Point(-10, -11, 17), 10, mirror);
		    objects.emplace_back(Point(-10, 10, 34), 16, red);
            objects.emplace_back(Point(15, 10, 31), 15, green);
            objects.emplace_back(Point(5, -5, 11), 3, pastel);

		    lights.push_back(Light(1, 0.8, Point(15,10,0)));
            lights.push_back(Light(1, 0.3, Point(0,10,5)));
		    lights.push_back(Light(2, 0.2, Vector(1,1,-5)));
		    lights.push_back(Light(0, 0.05));


		    Camera camera(Point(0,0,-7), Vector(0,0,1), 60);

		    render(image, camera);

		    std::cout << "Scene 0" << std::endl;

		    return true;
		}
		case 1:
		{
            /*
			objects.emplace_back(Point(2, -1, 12), 1, 10, 0.05, Color(70,30,100));
		    objects.emplace_back(Point(0, 0, 17), 4, 200, 0.2, Color(0,0,200));
		    objects.emplace_back(Point(-8, -9, 16), 6, 400, 0.6, Color(60,60,60));
		    objects.emplace_back(Point(-10, 10, 30), 16, 300, 0.3, Color(0,128,0));


		    lights.push_back(Light(1, 0.8, Point(15,10,0)));
		    lights.push_back(Light(2, 0.2, Vector(1,1,-5)));
		    lights.push_back(Light(0, 0.1));
            */

		    Camera camera(Point(0,0,-10), Vector(0,0,1), 60);

		    render(image, camera);

			std::cout << "Scene 1" << std::endl;

			return true;
		}
		case 2:
		{
			std::cout << "Scene 2" << std::endl;

			return true;
		}
		case 3:
		{
			std::cout << "Scene 3" << std::endl;

			return true;
		}
		default:
		{
			std::cout << "Bad scene" << std::endl;

			return false;
		}
	}

    return false;
}