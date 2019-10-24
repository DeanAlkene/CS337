//
// Created by dean on 19-10-20.
//

#ifndef LAB02_CAMERA_H
#define LAB02_CAMERA_H

#include "Utils.h"
#include "Object.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;
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
    float deltaYaw;

    float moveSpeed;
    float sensitivity;
    float zoom;

    void update()
    {
        front.x = cos(glm::radians(yaw + deltaYaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw + deltaYaw)) * cos(glm::radians(pitch));

        front = glm::normalize(front);

        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
public:
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 frt = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f), float Yaw = YAW, float Pitch = PITCH)
            :roll(0.0f), moveSpeed(SPEED), sensitivity(SENSITIVITY), zoom(ZOOM)
    {
        position = pos;
        front = frt;
        worldUp = worldup;
        yaw = Yaw;
        pitch = Pitch;
        deltaYaw = 0.0f;
        update();
    }

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float Yaw = YAW, float Pitch = PITCH)
            :roll(0.0f), front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED), sensitivity(SENSITIVITY), zoom(ZOOM)
    {
        position = glm::vec3(posX, posY, posZ);
        worldUp = glm::vec3(upX, upY, upZ);
        yaw = Yaw;
        pitch = Pitch;
        deltaYaw = 0.0f;
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

    void updatePos(const glm::vec3 &pos)
    {
        position = pos;
    }

    void updateFront(const glm::vec3 &dir)
    {
        deltaYaw = glm::acos(glm::dot(glm::normalize(dir), glm::vec3(0.0f, 0.0f, 1.0f)));
        deltaYaw = deltaYaw * 180 / glm::pi<float>();
        if(dir.x > 0.0f)
            deltaYaw = 360.0f - deltaYaw;
        std::cout << deltaYaw << ' ' << deltaYaw + yaw << std::endl;
        update();
    }

    void processKeyboard(Movement dir, float deltaTime)
    {
        float speed = moveSpeed * deltaTime;
        if(dir == Z)
            position += front * speed;
        if(dir == X)
            position -= front * speed;
        if(dir == UP)
            position += up * speed;
        if(dir == DOWN)
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
};

#endif //LAB02_CAMERA_H
