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

    Window(const char* name)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, name, NULL, NULL);
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
};

#endif //LAB02_WINDOW_H
