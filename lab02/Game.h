//
// Created by dean on 19-10-25.
//

#ifndef LAB02_GAME_H
#define LAB02_GAME_H

#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include "Skybox.h"
#include "Car.h"
#include "Scene.h"
#include "Road.h"
#include "Window.h"
#include "Barrier.h"
#include "Shadow.h"

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

class Game
{
public:
    float lastX;
    float lastY;

    float deltaTime;
    float lastFrame;
    bool firstMouse;
    bool cameraType;
    bool cursorState;
    bool collisionDetectState;
    bool shadowState;

    Window window;

    Camera camera[2];

    glm::vec3 lightPos;

    Shader shader_scene;
    Shader shader_road;
    Shader shader_car;
    Shader shader_skybox;
    Shader shader_shadow;
    Shader shader_debug;

    Road road;
    Barrier barrier;
    Car car;
    Skybox skybox;
    Scene scene;
    Shadow shadow;

    void configShader(Shader &shader)
    {
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setVec3("light.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setVec3("viewPos", camera[cameraType].getPosition());
        shader.setBool("shadows", shadowState);
        shader.setFloat("alpha", ALPHA);
        shader.setMat4("lightSpaceMatrix", shadow.getLightSpaceMatrix());
        shader.setInt("texture_shadow", 3);
        if(shadowState)
        {
            glActiveTexture(GL_TEXTURE0 + 3);
            glBindTexture(GL_TEXTURE_2D, shadow.getShadowTexture());
        }
    }

    void configScene()
    {
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);

        configShader(shader_scene);

        view = camera[cameraType].getViewMatrix();
        projection = glm::perspective(glm::radians(camera[cameraType].getZoom()), window.getAspect(), 0.1f, 1000.0f);
        shader_scene.setMat4("model", model);
        shader_scene.setMat4("view", view);
        shader_scene.setMat4("projection", projection);
    }
    
    void configRoad()
    {
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);

        configShader(shader_road);

        view = camera[cameraType].getViewMatrix();
        projection = glm::perspective(glm::radians(camera[cameraType].getZoom()), window.getAspect(), 0.1f, 1000.0f);
        shader_road.setMat4("model", model);
        shader_road.setMat4("view", view);
        shader_road.setMat4("projection", projection);
    }
    
    void configCar()
    {
        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        
        configShader(shader_car);

        model = car.getModel();
        view = camera[cameraType].getViewMatrix();
        projection = glm::perspective(glm::radians(camera[cameraType].getZoom()), window.getAspect(), 0.1f, 1000.0f);
        shader_car.setMat4("model", model);
        shader_car.setMat4("view", view);
        shader_car.setMat4("projection", projection);
    }
    
    void configSkybox()
    {
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        view = glm::mat4(glm::mat3(camera[cameraType].getViewMatrix()));
        projection = glm::perspective(glm::radians(camera[cameraType].getZoom()), window.getAspect(), 0.1f, 1000.0f);
        shader_skybox.setMat4("view", view);
        shader_skybox.setMat4("projection", projection);
    }

public:
    Game()
    : window("Running Kart"),
      shader_scene("./vertex.glsl", "./fragment.glsl"),
      shader_road("./vertex.glsl", "./fragment_NT.glsl"),
      shader_car("./vertex.glsl", "./fragment_NT.glsl"),
      shader_skybox("./vertex_skb.glsl", "./fragment_skb.glsl"),
      shader_shadow("./vertex_shadow.glsl", "./fragment_shadow.glsl"),
      shader_debug("./vertex_debug.glsl", "./fragment_debug.glsl"),
      road(std::string("/home/dean/CS337/Models/Scene/Roads/Roads01.obj")),
      barrier(std::string("/home/dean/CS337/Models/Scene/Barrier01.obj")),
      car(std::string("/home/dean/CS337/Models/Scene/Car/Car01.obj"), 1.0, 1.0),
      skybox(std::string("/home/dean/CS337/Models/Scene/Skybox"), std::string(".tga")),
      scene()
    {
        //camera[0] = Camera(glm::vec3(35.0f, 80.0f, 55.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f);
        lightPos = glm::vec3(-8.0f, 10.0f, -2.0f);
        //lightPos = glm::vec3(200.0f, 100.0f, 200.0f);
        camera[0] = Camera(glm::vec3(-3.0f, 5.0f, -5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, -45.0f);
        shadow.setMatrices(lightPos);
        lastX = WINDOW_WIDTH / 2.0f;
        lastY = WINDOW_HEIGHT / 2.0f;
        deltaTime = 0.0f;
        lastFrame = 0.0f;
        firstMouse = true;
        cameraType = false;
        cursorState = true;
        collisionDetectState = true;
        shadowState = true;
    }
    void init()
    {
        glfwMakeContextCurrent(window.window);
        glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window.window, mouse_callback);
        glfwSetScrollCallback(window.window, scroll_callback);

        glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glfwSetWindowUserPointer(window.window, this);
    }

    void renderLoop()
    {
        while(!glfwWindowShouldClose(window.window))
        {
            //Keyboard Input
            processInput(window.window);

            float timeValue = glfwGetTime();
            deltaTime = timeValue - lastFrame;
            lastFrame = timeValue;
            camera[1].updatePos(car.getCameraPos());
            camera[1].updateFront(car.getDir());

            glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            /*------------------------------------------------------------------------------*/
            if(shadowState)
            {
                glm::mat4 model = glm::mat4(1.0f);
                shader_shadow.activate();
                shader_shadow.setMat4("lightSpaceMatrix", shadow.getLightSpaceMatrix());
                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, shadow.getFrameBufferFBO());
                glClear(GL_DEPTH_BUFFER_BIT);
                glCullFace(GL_FRONT);
                shader_shadow.setMat4("model", model);
                scene.Draw(shader_shadow);
                shader_shadow.setMat4("model", model);
                road.Draw(shader_shadow);
                model = car.getModel();
                shader_shadow.setMat4("model", model);
                car.Draw(shader_shadow);
                glCullFace(GL_BACK);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                shader_shadow.deactivate();
            }
            /*------------------------------------------------------------------------------*/
        //    glViewport(0, 0, window.width, window.height);
        //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //    shader_debug.activate();
        //    shader_debug.setInt("depthMap", 3);
        //    glActiveTexture(GL_TEXTURE0 + 3);
        //    glBindTexture(GL_TEXTURE_2D, shadow.getShadowTexture());
        //    shadow.debugDraw();
        //    shader_debug.deactivate();
            /*------------------------------------------------------------------------------*/
            glViewport(0, 0, window.width, window.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader_scene.activate();
            configScene();
            scene.Draw(shader_scene);
            shader_scene.deactivate();

            shader_road.activate();
            configRoad();
            road.Draw(shader_road);
            shader_road.deactivate();

            shader_car.activate();
            configCar();
            car.Draw(shader_car);
            shader_car.deactivate();

            glDepthFunc(GL_LEQUAL);
            shader_skybox.activate();
            configSkybox();
            skybox.Draw(shader_skybox);
            glDepthFunc(GL_LESS);
            /*------------------------------------------------------------------------------*/
            glfwPollEvents();
            glfwSwapBuffers(window.window);
        }
    }

    void endGame()
    {
        glfwTerminate();
    }

    void processInput(GLFWwindow* window)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            camera[cameraType].processKeyboard(UP, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            camera[cameraType].processKeyboard(DOWN, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            camera[cameraType].processKeyboard(LEFT, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            camera[cameraType].processKeyboard(RIGHT, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
            camera[cameraType].processKeyboard(Z, deltaTime);
        if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
            camera[cameraType].processKeyboard(X, deltaTime);

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            car.processKeyboard(W, deltaTime);
            if(collisionDetectState)
                car.collisionDetect(barrier, W);
        }
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            car.processKeyboard(A, deltaTime);
            if(collisionDetectState)
                car.collisionDetect(barrier, A);
        }
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            car.processKeyboard(S, deltaTime);
            if(collisionDetectState)
                car.collisionDetect(barrier, S);
        }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            car.processKeyboard(D, deltaTime);
            if(collisionDetectState)
                car.collisionDetect(barrier, D);
        }
        if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            car.processKeyboard(Q, deltaTime);
        }
        if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            car.processKeyboard(E, deltaTime);
        }

        if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
            switchCollisionDetect();
        if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
            switchView();
        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            switchCursorPos(window);
        if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
            switchShadowState();
    }

    void switchView()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        cameraType = !cameraType;
    }

    void switchCursorPos(GLFWwindow* window)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        cursorState = !cursorState;
        if(cursorState)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void switchCollisionDetect()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        collisionDetectState = !collisionDetectState;
    }

    void switchShadowState()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        shadowState = !shadowState;
    }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    static_cast<Game*>(glfwGetWindowUserPointer(window))->window.width = width;
    static_cast<Game*>(glfwGetWindowUserPointer(window))->window.height = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(static_cast<Game*>(glfwGetWindowUserPointer(window))->firstMouse)
    {
        static_cast<Game*>(glfwGetWindowUserPointer(window))->lastX = xpos;
        static_cast<Game*>(glfwGetWindowUserPointer(window))->lastY = ypos;
    }

    float xoffset = xpos - static_cast<Game*>(glfwGetWindowUserPointer(window))->lastX;
    float yoffset = static_cast<Game*>(glfwGetWindowUserPointer(window))->lastY - ypos;
    static_cast<Game*>(glfwGetWindowUserPointer(window))->lastX = xpos;
    static_cast<Game*>(glfwGetWindowUserPointer(window))->lastY = ypos;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
        static_cast<Game*>(glfwGetWindowUserPointer(window))->camera[static_cast<Game*>(glfwGetWindowUserPointer(window))->cameraType].processMouseMovement(xoffset, yoffset);

    if(static_cast<Game*>(glfwGetWindowUserPointer(window))->firstMouse)
    {
        static_cast<Game*>(glfwGetWindowUserPointer(window))->firstMouse = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    static_cast<Game*>(glfwGetWindowUserPointer(window))->camera[static_cast<Game*>(glfwGetWindowUserPointer(window))->cameraType].processMouseScroll(yoffset);
}

#endif //LAB02_GAME_H
