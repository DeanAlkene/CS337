//
// Created by dean on 19-10-26.
//

#ifndef LAB02_SCENE_H
#define LAB02_SCENE_H

#include "Object.h"

class Scene
{
    friend class Game;
private:
    Object lights;
    Object stopSigns;
    Object speedLimit;
    Object trafficLight;
    Object trees;
    Object ground;
public:
    Scene()
            : lights(std::string("/home/dean/CS337/Models/Scene/Lights01.obj")),
              stopSigns(std::string("/home/dean/CS337/Models/Scene/StopSign/StopSign01.obj")),
              speedLimit(std::string("/home/dean/CS337/Models/Scene/SpeedLimit/SpeedLimit01.obj")),
              trafficLight(std::string("/home/dean/CS337/Models/Scene/trafficlight/trafficlight01.obj")),
              trees(std::string("/home/dean/CS337/Models/Scene/Palm/Palm01.obj")),
              ground(std::string("/home/dean/CS337/Models/Scene/Ground/Ground01.obj")) {}

    void Draw(Shader &shader) {
        lights.Draw(shader);
        stopSigns.Draw(shader);
        speedLimit.Draw(shader);
        trafficLight.Draw(shader);
        trees.Draw(shader);
        ground.Draw(shader);
    }
};
#endif //LAB02_SCENE_H
