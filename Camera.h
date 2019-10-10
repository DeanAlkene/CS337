//
// Created by dean on 19-10-6.
//

#ifndef LAB01_CAMERA_H
#define LAB01_CAMERA_H

#include "Utils.h"
#include "Object.h"

enum Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    Z,
    X
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.5f;
const float SENSITIVITY = 0.015f;
const float ZOOM = 45.0f;

class Camera
{
private:
    glm::vec3 position;
    glm::vec3 worldUp;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float yaw;
    float pitch;
    float roll;

    float moveSpeed;
    float sensitivity;
    float zoom;

    void update()
    {
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
public:
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH)
    :roll(0.0f), front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), sensitivity(SENSITIVITY), zoom(ZOOM)
    {
        position = pos;
        worldUp = worldup;
        yaw = Yaw;
        pitch = Pitch;
        update();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float Yaw = YAW, float Pitch = PITCH)
    :roll(0.0f), front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), sensitivity(SENSITIVITY), zoom(ZOOM)
    {
        position = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        yaw = Yaw;
        pitch = Pitch;
        update();
    }

    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    float getZoom()
    {
        return zoom;
    }

    glm::vec3 getPosition()
    {
        return position;
    }

    void processKeyboard(Movement dir, float deltaTime)
    {
        float speed = moveSpeed * deltaTime;
        if(dir == Z)
            position += front * speed;
        if(dir == X)
            position -= front * speed;
        if(dir == FORWARD)
            position += up * speed;
        if(dir == BACKWARD)
            position -= up * speed;
        if(dir == LEFT)
            position -= right * speed;
        if(dir == RIGHT)
            position += right * speed;
    }

    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if(constrainPitch)
        {
            if(pitch > 89.9f)
                pitch = 89.9f;
            if(pitch < -89.9f)
                pitch = -89.9f;
        }

        update();
    }

    void processMouseScroll(float yoffset)
    {
        if(zoom >= 1.0f && zoom <= 100.0f)
            zoom -= yoffset;
        if(zoom < 1.0f)
            zoom = 1.0f;
        if(zoom > 100.0f)
            zoom = 100.0f;
    }

    /*
    void processMouseDrag(CLICK_STATUS status, float xoffset, float yoffset)
    {
        if(status == LEFT_ONLY)
        {
            glm::vec3 dir = glm::vec3(-xoffset / (float)WINDOW_WIDTH, -yoffset / (float)WINDOW_HEIGHT, 0.0f);
            position += dir;
        }
        else if(status == LEFT_AND_RIGHT)
        {
            glm::vec3 axis = glm::vec3(-yoffset, xoffset, 0.0f);
            axis = glm::normalize(axis);
            float radius = glm::length(glm::cross(position, axis));
            //float projectLength = glm::dot(position, axis);
            float dist = glm::length(glm::vec2(xoffset, yoffset));
            //float angle = 2*glm::asin(dist / (2 * glm::pi<float>() * radius));
            float angle = glm::radians((float)dist * sensitivity * 2);
            //std::cout << radius << std::endl;
            //std::cout << dist << ' ' << radius << ' ' << angle << std::endl;
            glm::mat4 rot(1.0f);
            rot = glm::rotate(rot, angle, axis);
            //glm::vec3 pos = glm::vec3(rot * glm::vec4(position, 1.0f));
            //std::cout << pos.x << ' ' << pos.y << ' ' << pos.z << std::endl;
            //position = pos;
            position = glm::vec3(rot * glm::vec4(position, 1.0f));
            worldUp = glm::normalize(glm::vec3(rot * glm::vec4(worldUp, 1.0f)));
            update();

            //glm::vec3 axis = glm::normalize(glm::vec3(-yoffset, xoffset, 0.0f));
            //float dist = glm::length(glm::vec2(xoffset, yoffset));
            //float dist = xoffset;
            //float angle = 2*glm::asin(dist / diameter);
            //model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }
     */
};

#endif //LAB01_CAMERA_H
