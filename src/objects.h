#ifndef OBJECTS_H
#define OBJECTS_H


std::vector<Object*> objects;
std::vector<Light> lights;
Color Back_ground(15, 0, 35);

int envmap_width, envmap_height;
std::vector<Color> envmap;
Model model;


#endif