//
// Created by dean on 19-10-31.
//

#ifndef LAB02_ROAD_H
#define LAB02_ROAD_H

#include "Utils.h"
#include "Object.h"

class Road : public Object
{
public:
    Shadow shadow;
    Road(const std::string &path) : Object(path), shadow() {}

    glm::mat4 getModel() {return glm::mat4(1.0f);}
};

#endif //LAB02_ROAD_H
