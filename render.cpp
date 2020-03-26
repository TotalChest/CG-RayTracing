#include "properties.h"
#include "geometry.h"
#include "objects.h"



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


bool ClosestIntersection(Point &O, Vector &D, float t_min, float t_max, Point &P, Vector &N, Material &mat)
{
    float closest_t = INF;
    bool Intersection = false;
    Object* object;
    for(Object* obj: objects)
    {
        std::pair<float, float> t = obj->IntersectRay(O, D);
        if (t.first >= t_min and t.first <= t_max and t.first < closest_t)
        {
            closest_t = t.first;
            Intersection = true;
            object = obj;
        }
        if (t.second >= t_min and t.second <= t_max and t.second < closest_t)
        {
            closest_t = t.second;
            Intersection = true;
			object = obj;      
        }
    }

    if(Intersection)
    {
        P = D.to_point(closest_t) + O;
        N = object->get_normal(P);
        mat = object->material;
    }
/*

    if (fabs(D.y) > EPSILON)
    {
        float plane_dist = (-1.0)*(O.y + 5) / D.y;
        Point T = D.to_point(plane_dist) + O;
        if (plane_dist > 0 && fabs(T.x)<10 && T.z<30 && T.z>10 && plane_dist < closest_t){
            Intersection = true;
            P = T;
            N = Vector(0,1,0);
            mat = (int(0.5*T.x+10) + int(.5*T.z)) & 1 ? Material(Color(115,130,80), 600, 1, 0.2, 0, 1) : Material(Color(215,130,80), 600, 1, 0.2, 0, 1);
        }
    }
*/
    return Intersection;
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

           
    		Material mat;
            Point P_2;
            Vector N_2;
            if(ClosestIntersection(P, L, EPSILON, t_max, P_2, N_2, mat)){
                float k = (N * L)/(N.norm()*L.norm());
                d += l.intensity * std::max(0.f, k) * mat.refractive_index;
                continue;
            }


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
    Material mat;
    Point P;
    Vector N;

    if(!ClosestIntersection(O, D, t_min, t_max, P, N, mat))
        return Back_ground;

    Vector V = D * (-1.f);

    std::pair<float, float> light = ComputeLighting(P, N, V, mat.specular, mat.specular_index);
    Color local_color = mat.color * light.first;
    
    if(depth <= 0)
    	return local_color + Color(255,255,255) * light.second;

    float h = mat.refractive_index;
    float r = std::min(1 - h, mat.reflective);

    local_color = local_color * (1 - h - r);

    if(r > 0)
    {
        Vector R = ReflectRay(V, N);
        local_color = local_color + TraceRay(P, R, EPSILON, INF, depth - 1) * r;
    }
    
    if(h > 0)
    {
        Vector S = RefractRay(V, N, mat.refractive);
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
            Back_ground = Color(15, 0, 35);

            Material glass(Color(200,200,200), 100, 0.8, 0.2, 0.8, 5);
            Material red(Color(200,20,0), 2, 0.1, 0.05, 0, 1);
            Material mirror(Color(100,100,100), 800, 2, 0.8, 0, 1);
            Material green(Color(40,150,30), 200, 0.2, 0.3, 0, 1);
            Material pastel(Color(215,130,80), 600, 1, 0.2, 0, 1);

		    Sphere sphere1(Point(0, 0, 17), 4, glass);
		    Sphere sphere2(Point(-10, -11, 17), 10, mirror);
		    Sphere sphere3(Point(-10, 10, 34), 16, red);
            Sphere sphere4(Point(15, 10, 31), 15, green);
            Sphere sphere5(Point(5, -5, 11), 3, pastel);

            objects.push_back(&sphere1);
            objects.push_back(&sphere2);
            objects.push_back(&sphere3);
            objects.push_back(&sphere4);
            objects.push_back(&sphere5);

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
            Back_ground = Color(200, 197, 230);

            Material red_glass(Color(230,100,0), 600, 0.6, 0.05, 0.65, 1);
            Material dark_mirror(Color(10,60,70), 700, 0.8, 0.5, 0, 1);
            Material pastel(Color(120, 160, 215), 1, 0.05, 0, 0, 1);
            Material lamp(Color(255, 255, 255), 1000, 0, 0, 0.1, 1);

            Sphere sphere1(Point(6, -2, 15), 5, red_glass);
            Sphere sphere2(Point(0, 10.5, 15), 4, lamp);
            Sphere sphere3(Point(-8, -4, 17), 3, dark_mirror);
            Plane plane1(Vector(0, 0, -1), Point(0, 0, 20), pastel);
            Plane plane2(Vector(-1, 0, 0), Point(11, 0, 0), pastel);
            Plane plane3(Vector(0, -1, 0), Point(0, 7, 0), pastel);
            Plane plane4(Vector(1, 0, 0), Point(-11, 0, 0), pastel);
            Plane plane5(Vector(0, 1, 0), Point(0, -7, 0), pastel);
            Plane plane6(Vector(0, 0, 1), Point(0, 0, -11), pastel);

		    objects.push_back(&sphere1);
            objects.push_back(&sphere3);
            objects.push_back(&sphere2);
            objects.push_back(&plane1);
            objects.push_back(&plane2);
            objects.push_back(&plane3);
            objects.push_back(&plane4);
            objects.push_back(&plane5);
            objects.push_back(&plane6);

		    lights.push_back(Light(1, 0.7, Point(0,6,15)));
		    lights.push_back(Light(0, 0.2));

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