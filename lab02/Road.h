//
// Created by dean on 19-10-26.
//

#ifndef LAB02_ROAD_H
#define LAB02_ROAD_H

#include "Object.h"
#include "Car.h"

class Road : public Object
{
private:
    Object roadEdge;
public:
    Road(const std::string &roadPath)
            : Object(roadPath) {}

    Road(const std::string &roadPath, const std::string &edgePath)
    : Object(roadPath), roadEdge(edgePath) {}

    bool collisionDetect(Car &car)
    {

    }
};

#endif //LAB02_ROAD_H
