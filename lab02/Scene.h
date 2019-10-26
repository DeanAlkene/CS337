//
// Created by dean on 19-10-26.
//

#ifndef LAB02_SCENE_H
#define LAB02_SCENE_H

#include "Object.h"
#include "Road.h"

class Scene
{
private:
    Object lights;
    Object stopSigns;
    Object speedLimit;
    Object trafficLight;
    Object trees;
public:
    Scene()
    : lights(std::string("/home/dean/CS337/Models/Scene/Lights.obj")),
      stopSigns(std::string("/home/dean/CS337/Models/Scene/StopSign/StopSign.obj")),
      speedLimit(std::string("/home/dean/CS337/Models/Scene/SpeedLimit/SpeedLimit.obj")),
      trafficLight(std::string("/home/dean/CS337/Models/Scene/trafficlight/trafficlight.obj")),
      trees(std::string("/home/dean/CS337/Models/Scene/Palm/Palm.obj")) {}

    void Draw(Shader &shader)
    {
        lights.Draw(shader);
        stopSigns.Draw(shader);
        speedLimit.Draw(shader);
        trafficLight.Draw(shader);
        trees.Draw(shader);
    }
};

#endif //LAB02_SCENE_H
