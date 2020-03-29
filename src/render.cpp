#include "properties.h"
#include "geometry.h"
#include "model.h"
#include "objects.h"



Vector ReflectRay(Vector &V, Vector &N) {
	Vector C  = N* (N * V * 2.f);
    C = C + (V * (-1.f));
    return C;
}


bool RefractRay(Vector &V, Vector &N, float &refractive, Vector &S) {
    float cos = (-1.0)*(V * N)/(V.norm() * N.norm());
    float n1 = 1, n2 = refractive;
    Vector n = N;
    if (cos < 0) {
        cos = -cos;
        std::swap(n1 , n2);
        n = N * (-1);
    }
    float A = n1 / n2;
    float k = 1.0 - A*A*(1.0 - cos*cos);
    if(k <= 0)
        return false;
    S = V*A + n*(A*cos - sqrt(k));
    return true;
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

    if(model.exist)
    {
        float model_dist = INF;
        for (int t=0; t<model.nfaces(); t++)
        {
            float dist;
            if (model.ray_triangle_intersect(t, O, D, dist) && dist < closest_t)
            {
                Intersection = true;
                closest_t = dist;
                P = D.to_point(closest_t) + O;
                Point v0 = model.point(model.vert(t, 0));
                Point v1 = model.point(model.vert(t, 1));
                Point v2 = model.point(model.vert(t, 2));
                N = cross(v1-v0, v2-v0);
                N = N / N.norm();
                mat = model.material;
            }
        }
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
            if(ClosestIntersection(P, L, EPSILON, t_max, P_2, N_2, mat) && (Point(0,0,0) - P_2).norm() < 95){
                float k = (N * L)/(N.norm()*L.norm());
                d += l.intensity * std::max(0.f, k) * mat.refractive_index;
                if (specular != -1)
                {
                    Vector R = ReflectRay(L, N);
                    k = (R * V)/(R.norm() * V.norm());
                    if (k > 0.f)
                        s += l.intensity * specular_index * pow(k , specular) * mat.refractive_index;
                }
                continue;
            }

            float k = (N * L)/(N.norm()*L.norm());
            d += l.intensity * std::max(0.f, fabs(k));


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
    if((Point(0,0,0) - P).norm() > 95)
    {
        if(envmap.size())
        {
            int a = (atan2(P.z, P.x) / (2*PI) + .5) * envmap_width;
            int b = acos(P.y / 100) / PI * envmap_height;
            return envmap[a+b*envmap_width];
        }
        return Back_ground;
    }

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
        Vector S(0,0,0);
        if(RefractRay(D, N, mat.refractive, S))
            local_color = local_color + TraceRay(P, S, EPSILON, INF, depth - 1) * h;
    }

    return local_color + Color(255,255,255) * light.second;
}



void render(std::vector<uint32_t> &image, Camera &camera)
{
	omp_set_num_threads(threads);
    std::cout << "Threads: " << threads << std::endl;
	
	for(int i = (-1)*HEIGHT/2; i < HEIGHT/2; ++i)
	{
        if((i+HEIGHT/2)%(HEIGHT/10) == 0)
            std::cout << "\rProgress: " <<  (i+HEIGHT/2)/(HEIGHT/100) << "%" << std::flush;
        #pragma omp parallel for
    	for(int j = (-1)*WIDTH/2; j < WIDTH/2; ++j)
    	{
        	Vector D = camera.point_to_vector(i, j);
        	Color color = TraceRay(camera.O, D, 1, INF, RECURSION_DEPTH);
        	image[(i+HEIGHT/2)*WIDTH + (j+WIDTH/2)] = color.hex();
    	}
	}
    std::cout << "\rProgress: 100%\n";

   return;
}


bool build_image(std::vector<uint32_t> &image, int sceneId)
{

	switch(sceneId)
	{
		case 1:
		{
            std::cout << "Scene 1" << std::endl;

            Back_ground = Color(15, 0, 35);

            int n = -1;
            unsigned char *data = stbi_load((TEXTURES_DIR + "space.jpg").c_str(), &envmap_width, &envmap_height, &n, 0);
            if (!data || 3!=n) {
                std::cerr << "Error: can not load the environment map" << std::endl;
                return -1;
            }
            envmap = std::vector<Color>(envmap_width*envmap_height);
            for (int j = 0; j<envmap_height; j++)
                for (int i = 0; i<envmap_width; i++)
                    envmap[i+j*envmap_width] = Color(data[(i+j*envmap_width)*3+0], data[(i+j*envmap_width)*3+1], data[(i+j*envmap_width)*3+2]) * 0.4;
            stbi_image_free(data);
            Sphere env(Point(0, 0, 0), 100, Material());

            Material glass(Color(200,200,200), 200, 0.8, 0.2, 0.8, 3);
            Material red(Color(200,20,0), 2, 0.1, 0.04, 0, 1);
            Material mirror(Color(100,100,100), 800, 2, 0.8, 0, 1);
            Material green(Color(40,150,30), 200, 0.2, 0.3, 0, 1);
            Material pastel(Color(215,130,80), 600, 1, 0.2, 0, 1);

		    Sphere sphere1(Point(0, 0, 17), 4, glass);
		    Sphere sphere2(Point(-10, -11, 17), 10, mirror);
		    Sphere sphere3(Point(-10, 10, 34), 16, red);
            Sphere sphere4(Point(15, 10, 31), 15, green);
            Sphere sphere5(Point(5, -5, 11), 3, pastel);

            objects.push_back(&env);
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

		    return true;
		}
		case 2:
		{
            std::cout << "Scene 2" << std::endl;

            Back_ground = Color(200, 197, 230);

            Material red_glass(Color(240,40,10), 600, 0.6, 0.05, 0.75, 1.05);
            Material green_glass(Color(10,100,20), 600, 0.6, 0.05, 0.75, 1.00);
            Material dark_mirror(Color(10,60,70), 700, 0.8, 0.5, 0, 1);
            Material pastel(Color(170, 125, 80), 0.4, 0.02, 0, 0, 1);
            Material dark_pastel(Color(145, 90, 40), 0.5, 0.05, 0, 0, 1);
            Material lamp(Color(255, 255, 255), 10, 1, 0, 0, 1);

            Sphere sphere1(Point(6, -2, 15), 5, red_glass);
            Sphere sphere2(Point(-8, -4, 17), 3, dark_mirror);
            Plane plane1(Vector(0, 0, -1), Point(0, 0, 20), pastel);
            Plane plane2(Vector(-1, 0, 0), Point(11, 0, 0), pastel);
            Plane plane3(Vector(0, -1, 0), Point(0, 7, 0), dark_pastel);
            Plane plane4(Vector(1, 0, 0), Point(-11, 0, 0), pastel);
            Plane plane5(Vector(0, 1, 0), Point(0, -7, 0), dark_pastel);
            Plane plane6(Vector(0, 0, 1), Point(0, 0, -11), pastel);
            Triangle triangle1(Point(-5, -7, 8), Point(-2, -3, 12), Point(2, -7, 9), green_glass);
            Triangle triangle2(Point(-5, -7, 8), Point(-2, -3, 12), Point(-3, -7, 14), green_glass);
            Triangle triangle3(Point(-3, -7, 14), Point(-2, -3, 12), Point(2, -7, 9), green_glass);

		    objects.push_back(&sphere1);
            objects.push_back(&sphere2);
            objects.push_back(&plane1);
            objects.push_back(&plane2);
            objects.push_back(&plane3);
            objects.push_back(&plane4);
            objects.push_back(&plane5);
            objects.push_back(&plane6);
            objects.push_back(&triangle1);
            objects.push_back(&triangle2);
            objects.push_back(&triangle3);

		    lights.push_back(Light(1, 0.4, Point(0,2,15)));
            lights.push_back(Light(1, 0.4, Point(0,2,-5)));
		    lights.push_back(Light(0, 0.2));

		    Camera camera(Point(0,0,-10), Vector(0,0,1), 70);

		    render(image, camera);

			return true;
		}
		case 3:
		{
            std::cout << "Scene 3" << std::endl;

            Back_ground = Color(200, 200, 200);

            int n = -1;
            unsigned char *data = stbi_load((TEXTURES_DIR + "space.jpg").c_str(), &envmap_width, &envmap_height, &n, 0);
            if (!data || 3!=n)
            {
                std::cerr << "Error: can not load the environment map" << std::endl;
                return -1;
            }
            envmap = std::vector<Color>(envmap_width*envmap_height);
            for (int j = 0; j<envmap_height; j++)
                for (int i = 0; i<envmap_width; i++)
                    envmap[i+j*envmap_width] = Color(data[(i+j*envmap_width)*3+0], data[(i+j*envmap_width)*3+1], data[(i+j*envmap_width)*3+2]) * 0.5;
            stbi_image_free(data);
            Sphere env(Point(0, 0, 0), 100, Material());

            model = Model("rocket.obj");
            model.material = Material(Color(170, 160, 210), 200, 0.6, 0.1, 0, 1);

            objects.push_back(&env);

            lights.push_back(Light(1, 0.5, Point(10,10,-35)));
            lights.push_back(Light(1, 0.3, Point(-5,-30,-10)));
            lights.push_back(Light(1, 0.4, Point(-20,50,-40)));
            lights.push_back(Light(0, 0.05));

            Camera camera(Point(0,0,-40), Vector(0,0,1), 90);

            render(image, camera);

			return true;
		}
		case 4:
		{
			std::cout << "Scene 4" << std::endl;

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