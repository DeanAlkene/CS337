//
// Created by dean on 19-10-25.
//

#ifndef LAB02_WINDOW_H
#define LAB02_WINDOW_H

#include "Utils.h"

class Window
{
public:
    GLFWwindow* window;
    int width;
    int height;

    Window(const char* name, int w = WINDOW_WIDTH, int h = WINDOW_HEIGHT)
    {
        width = w;
        height = h;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, name, NULL, NULL);
        if(!window)
        {
            std::cerr << "Failed to create window!" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(window);

        if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(1);
        }
    }

    float getAspect()
    {
        return ((float)width / (float) height);
    }
};

#endif //LAB02_WINDOW_H
