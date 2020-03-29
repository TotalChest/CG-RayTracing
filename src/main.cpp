#include <iostream>
#include <cstdint>

#include <string>
#include <vector>
#include <unordered_map>

#include "Bitmap.h"


extern const int HEIGHT;
extern const int WIDTH;
extern int threads;
extern int sceneId;
bool build_image(std::vector<uint32_t> &, int);


int main(int argc, const char** argv)
{

    std::unordered_map<std::string, std::string> cmdLineParams;

    for(int i=0; i<argc; i++)
    {
        std::string key(argv[i]);

        if(key.size() > 0 && key[0]=='-')
        {
            if(i != argc-1)
            {
            cmdLineParams[key] = argv[i+1];
            i++;
            }
        else
            cmdLineParams[key] = "";
        }
    }

    if(cmdLineParams.find("-scene") != cmdLineParams.end())
        sceneId = atoi(cmdLineParams["-scene"].c_str());

    std::string outFilePath;
    if(cmdLineParams.find("-out") != cmdLineParams.end())
        outFilePath = cmdLineParams["-out"];
    else
        outFilePath = std::string("Scene_") + std::to_string(sceneId) + ".bmp";

    
    if(cmdLineParams.find("-threads") != cmdLineParams.end())
        threads = atoi(cmdLineParams["-threads"].c_str());


    std::vector<uint32_t> image(HEIGHT * WIDTH, 0); 
    
    if(build_image(image, sceneId))
        SaveBMP(outFilePath.c_str(), image.data(), WIDTH, HEIGHT);


    std::cout << "Done." << std::endl;

    return 0;
}