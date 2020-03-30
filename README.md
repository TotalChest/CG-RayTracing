# Computer Graphics - Ray Tracing 
CMC course "Сomputer graphics"

### Build:
```bash
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j 4
```
### Run:
```bash
$ ./rt -out <output_path> -scene <scene_number> -threads <threads>
```
### Features:
- Base
	- Phong model
	- Shadows
	- Specular
	- Reflective
	- Spheres and triangles
	- Light sources
	- 1600*900 pixels
- Additions
	- Plane (+1)
	- Textures (+1)
	- Refractive (+1)
	- Threads(+2)
	- Enviroment map (+1)
	- 3D models (+4)
### Examples: 
![Example](examples/Scene_1.bmp)
![Example](examples/Scene_2.bmp)
![Example](examples/Scene_3.bmp)
