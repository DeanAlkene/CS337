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
    float turnRadius;
    glm::mat4 model;
    glm::vec3 leftTurnCenter;
    glm::vec3 rightTurnCenter;
    glm::vec3 frontAnchor;
    glm::vec3 backAnchor;
    glm::vec3 camPos;
    std::vector<glm::vec3> AABB;
    void update(const glm::mat4 &delta)
    {
        leftTurnCenter = glm::vec3(delta * glm::vec4(leftTurnCenter, 1.0f));
        rightTurnCenter = glm::vec3(delta * glm::vec4(rightTurnCenter, 1.0f));
        frontAnchor = glm::vec3(delta * glm::vec4(frontAnchor, 1.0f));
        backAnchor = glm::vec3(delta * glm::vec4(backAnchor, 1.0f));
    }
public:
    Car(const std::string &path, float v, float a) : Object(path), velocity(v), acceleration(a)
    {
        model = glm::mat4(1.0f);
        getAABB();
        leftTurnCenter = AABB[5] + glm::vec3(0.0f, 0.0f, -1.5f);
        leftTurnCenter = glm::vec3(leftTurnCenter.x, 0.0f, leftTurnCenter.z);
        rightTurnCenter = AABB[4] + glm::vec3(0.0f, 0.0f, -1.5f);
        rightTurnCenter = glm::vec3(rightTurnCenter.x, 0.0f, rightTurnCenter.z);
        frontAnchor = glm::vec3(0.0f, 0.0f, 0.0f);
        backAnchor = glm::vec3(0.0f, 0.0f, -1.0f);
        camPos = glm::vec3(0.0f, 2.0f, -2.5f);
        turnRadius = 20.0f;
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

    glm::mat4 getModel()
    {
        return model;
    }

    glm::vec3 getDir()
    {
        return (frontAnchor - backAnchor);
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
        glm::mat4 axisTrans(1.0f);
        axisTrans = glm::translate(axisTrans, leftTurnCenter);

        float angularVelocity = velocity / turnRadius;
        float angle = angularVelocity * deltaTime;

        glm::mat4 move(1.0f);
        move = axisTrans * glm::rotate(move, angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::inverse(axisTrans);
        model = move * model;
        update(move);
    }

    void turnRight(float deltaTime)
    {
        glm::mat4 axisTrans(1.0f);
        axisTrans = glm::translate(axisTrans, rightTurnCenter);

        float angularVelocity = velocity / turnRadius;
        float angle = angularVelocity * deltaTime;

        glm::mat4 move(1.0f);
        move = axisTrans * glm::rotate(move, -angle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::inverse(axisTrans);
        model = move * model;
        update(move);
    }

    void goForward(float deltaTime)
    {
        glm::mat4 move(1.0f);
        move = glm::translate(move, -deltaTime * velocity * (frontAnchor - backAnchor));
        model = move * model;
        //printMat4(model);
        update(move);
    }

    void goBackward(float deltaTime)
    {
        glm::mat4 move(1.0f);
        move = glm::translate(move, deltaTime * velocity * (frontAnchor - backAnchor));
        model = move * model;
        //printMat4(model);
        update(move);
    }
};

#endif //LAB02_CAR_H
