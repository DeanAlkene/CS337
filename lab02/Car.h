//
// Created by dean on 19-10-24.
//

#ifndef LAB02_CAR_H
#define LAB02_CAR_H


#include "Object.h"
#include "Barrier.h"

const float epsilon = 0.2f;

class Car : public Object
{
private:
    float velocity;
    float acceleration;
    float turnRadius;
    float length;
    float height;
    float width;
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
        for(int i = 0; i < AABB.size(); ++i)
        {
            AABB[i] = glm::vec3(delta * glm::vec4(AABB[i], 1.0f));
        }
    }
public:
    Car(const std::string &path, float v, float a) : Object(path), velocity(v), acceleration(a)
    {
        model = glm::mat4(1.0f);
        getAABB();
        leftTurnCenter = AABB[6] + glm::vec3(0.0f, 0.0f, length / 8.0f);
        leftTurnCenter = glm::vec3(leftTurnCenter.x, 0.0f, leftTurnCenter.z);
        rightTurnCenter = AABB[7] + glm::vec3(0.0f, 0.0f, length / 8.0f);
        rightTurnCenter = glm::vec3(rightTurnCenter.x, 0.0f, rightTurnCenter.z);
        frontAnchor = glm::vec3(0.0f, 0.0f, 0.0f);
        backAnchor = glm::vec3(0.0f, 0.0f, -1.0f);
        camPos = glm::vec3(0.0f, height * 7.0f / 8.0f, AABB[6].z + length / 4.0f);
        turnRadius = 10.0f;
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
        AABB.push_back(glm::vec3(max_x, max_y, max_z)); //top tail left
        AABB.push_back(glm::vec3(min_x, max_y, max_z)); //top tail right
        AABB.push_back(glm::vec3(min_x, max_y, min_z)); //top head right
        AABB.push_back(glm::vec3(max_x, max_y, min_z)); //top head left
        AABB.push_back(glm::vec3(max_x, min_y, max_z)); //bottom tail left
        AABB.push_back(glm::vec3(min_x, min_y, max_z)); //bottom tail right
        AABB.push_back(glm::vec3(min_x, min_y, min_z)); //bottom head right?
        AABB.push_back(glm::vec3(max_x, min_y, min_z)); //bottom head left?
        length = max_z - min_z;
        height = max_y - min_y;
        width = max_x - min_x;
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
        update(move);
    }

    void goBackward(float deltaTime)
    {
        glm::mat4 move(1.0f);
        move = glm::translate(move, deltaTime * velocity * (frontAnchor - backAnchor));
        model = move * model;
        update(move);
    }

    void collisionDetect(Barrier &barrier, Movement move)
    {
        float angle;
        bool detected = false;
        for(int i = 4; i < 8; ++i)
        {
            for(auto it : barrier.segments)
            {
                float dist = it.calcDist(Point2D(AABB[i].x, AABB[i].z));
                if(dist < 0)
                    continue;
                if(dist < epsilon)
                {
                    glm::vec3 carDir = glm::normalize(this->getDir());
                    carDir.y = 0.0f;
                    angle = glm::acos(glm::dot(carDir, it.dir)) * 180.0f / glm::pi<float>();
                    if(angle > 90.0f)
                    {
                        angle = 180.0f - angle;
                    }
                    detected = true;
                    break;
                }
            }
            if(detected)
                break;
        }
        if(detected)
        {
            float factor = 0.05f;
            if(move == W)
            {
                goBackward(factor);
            }
            else if(move == S)
            {
                goForward(factor);
            }
            else if(move == A)
            {
                if(angle < 45.0f)
                    turnRight(factor);
                else
                    goBackward(factor / 2.0f);
            }
            else if(move == D)
            {
                if(angle < 45.0f)
                    turnLeft(factor);
                else
                    goBackward(factor / 2.0f);
            }

        }
    }
};

#endif //LAB02_CAR_H
