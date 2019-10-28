//
// Created by dean on 19-10-26.
//

#ifndef LAB02_ROAD_H
#define LAB02_ROAD_H

#include "Object.h"
#include "Car.h"
#include "Barrier.h"

const float epsilon = 1.0e-3f;

class Road : public Object
{
private:
    Barrier barrier;
public:
    Road(const std::string &roadPath, const std::string &barrierPath)
    : Object(roadPath), barrier(barrierPath) {}

    void collisionDetect(Car &car)
    {
        for(int i = 4; i < 8; ++i)
        {
            for(auto it : barrier.segments)
            {
                float dist = it.calcDist(Point2D(car.AABB[i].x, car.AABB[i].z));
                if(dist < 0)
                    continue;
                if(dist < epsilon)
                {
//                    std::cout << "Collision Detected! " << dist << std::endl;
//                    std::cout << it.p1 << ' ' << it.p2 << ' ' << it.A << ' ' << it.B << ' ' << it.C << std::endl;
//                    std::cout << car.AABB[i].x << ' ' << car.AABB[i].z << std::endl; //DEBUG
//                    std::cout << std::endl;
                    glm::vec3 carDir = glm::normalize(car.getDir());
                    carDir.y = 0.0f;
                    float angle = glm::acos(glm::dot(carDir, it.dir));
                    if(angle > 90.0f)
                    {
                        angle = 180.0f - angle;
                    }

                    if(i == 6 || i == 7)
                    {
                        float v_0 = car.getVelocity();
                        float v = -angle * v_0 / 90.0f + v_0;
                        car.setVelocity(v);
                        car.goBackward(0.05);
                        car.setVelocity(v_0);
                    }
                    else
                    {
                        float v_0 = car.getVelocity();
                        float v = -angle * v_0 / 90.0f + v_0;
                        car.setVelocity(v);
                        car.goForward(0.05);
                        car.setVelocity(v_0);
                    }
                }
            }
        }
    }
};

#endif //LAB02_ROAD_H
