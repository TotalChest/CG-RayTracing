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


float ComputeLighting(Point &P, Vector &N, Vector &V, int specular)
{
    float i = 0.0;
    for(Light & l : lights)
    {
        if (l.type == 0)
            i += l.intensity;
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
            i += l.intensity * std::max(0.f, k);


            if (specular != -1)
            {
            	Vector R = ReflectRay(L, N);
                k = (R * V)/(R.norm() * V.norm());
                if (k > 0.f)
                    i += l.intensity * 1.5 * pow(k , specular);
           	}

        }
    }
    return i;
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
    Color local_color = sphere.material.color * ComputeLighting(P, N, V, sphere.material.specular);

    float h = sphere.material.refractive.refract;
    float r = std::min(1 - h, sphere.material.reflective);
    
    if(depth <= 0 || r <= 0)
    	return local_color;

    Vector R = ReflectRay(V, N);
    Color reflected_color = TraceRay(P, R, EPSILON, INF, depth - 1);
    Vector S = RefractRay(V, N, sphere.material.refractive.index);
    Color refracted_color = TraceRay(P, S, EPSILON, INF, depth - 1);

    return local_color * (1 - r - h) + reflected_color * r + refracted_color * h;

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

			objects.emplace_back(Point(2, -1, 12), 1.5, Material(Color(250, 230, 0), 10, 0.05, Refractive(0.6,1)) );
		    objects.emplace_back(Point(0, 0, 17), 4, Material(Color(200,200,200), 800, 0.02, Refractive(0.9, 5)) );
		    objects.emplace_back(Point(-10, -11, 17), 10, Material(Color(100,100,100), 500, 0.8, Refractive(0,1)) );
		    objects.emplace_back(Point(-10, 10, 34), 16, Material(Color(255,0,0), 300, 0.3, Refractive(0,1)) );
            objects.emplace_back(Point(15, 10, 31), 15, Material(Color(50,200,40), 600, 0.5, Refractive(0,1)) );
            objects.emplace_back(Point(5, -5, 11), 3, Material(Color(215,130,80), 1000, 0.3, Refractive(0,1)) );

		    lights.push_back(Light(1, 0.8, Point(15,10,0)));
            lights.push_back(Light(1, 0.3, Point(0,10,5)));
		    lights.push_back(Light(2, 0.2, Vector(1,1,-5)));
		    lights.push_back(Light(0, 0.1));


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