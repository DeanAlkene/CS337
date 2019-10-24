//
// Created by dean on 19-10-24.
//

#ifndef LAB02_CAR_H
#define LAB02_CAR_H


#include "Object.h"

const float MAX = 1.0e-9;

class Car : public Object
{
private:
    float velocity;
    float acceleration;
    glm::mat4 model;
    glm::vec3 leftTurnAxis;
    glm::vec3 rightTurnAxis;
    glm::vec3 camPos;
    std::vector<glm::vec3> AABB;
    void update(const glm::mat4 &delta)
    {

    }
public:
    Car(const std::string &path, float v, float a) : Object(path), velocity(v), acceleration(a)
    {
        model = glm::mat4(1.0f);
        getAABB();
    }

    void getAABB()
    {
        float min_x = MAX, min_y = MAX, min_z = MAX, max_x = -MAX, max_y = -MAX, max_z = -MAX;
        for(int i = 0; i < meshes.size(); ++i)
        {
            for(int j = 0; j < meshes[i].vertices.size(); ++j)
            {
                min_x = std::min(meshes[i].vertices[j].Position.x, min_x);
                min_y = std::min(meshes[i].vertices[j].Position.y, min_y);
                min_z = std::min(meshes[i].vertices[j].Position.z, min_z);
                max_x = std::max(meshes[i].vertices[j].Position.x, max_x);
                max_y = std::max(meshes[i].vertices[j].Position.y, max_y);
                max_z = std::max(meshes[i].vertices[j].Position.z, max_z);
            }
        }
        std::cout << min_x << ' ' << min_y << ' ' << min_z << ' ' << max_x << ' ' << max_y << ' ' << max_z << std::endl;
        AABB.push_back(glm::vec3(max_x, max_y, max_z)); //top up left
        AABB.push_back(glm::vec3(min_x, max_y, max_z)); //top up right
        AABB.push_back(glm::vec3(min_x, max_y, min_z)); //top down right
        AABB.push_back(glm::vec3(max_x, max_y, min_z)); //top down left
        AABB.push_back(glm::vec3(max_x, min_y, max_z)); //bottom up left
        AABB.push_back(glm::vec3(min_x, min_y, max_z)); //bottom up right
        AABB.push_back(glm::vec3(min_x, min_y, min_z)); //bottom down right
        AABB.push_back(glm::vec3(max_x, min_y, min_z)); //bottom down left
    }

    glm::vec3 getCameraPos()
    {
        return glm::vec3(model * glm::vec4(camPos, 1.0f));
    }

    void processKeyboard(Movement dir, float deltaTime)
    {
        if(dir == A)
            turnLeft(deltaTime);
        else if(dir == D)
            turnRight(deltaTime);
        else if(dir == W)
            goForward(deltaTime);
        else if(dir == S)
            goBackward(deltaTime);
        else if(dir == Q)
            velocity += acceleration * deltaTime;
        else if(dir == E)
            velocity -= acceleration * deltaTime;

    }

    void turnLeft(float deltaTime)
    {

    }

    void turnRight(float deltaTime)
    {

    }

    void goForward(float deltaTime)
    {
        glm::mat4 move(1.0f);
        glm::translate(move, glm::vec3(0.0f, 0.0f, deltaTime * velocity));
        model = move * model;
        update(move);
    }

    void goBackward(float deltaTime)
    {
        glm::mat4 move(1.0f);
        glm::translate(move, glm::vec3(0.0f, 0.0f, -deltaTime * velocity));
        model = move * model;
        update(move);
    }
};

#endif //LAB02_CAR_H
