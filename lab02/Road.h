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
    Road(const std::string &path) : Object(path) {}
};

#endif //LAB02_ROAD_H
