//
// Created by dean on 19-10-20.
//

#ifndef LAB02_UTILS_H
#define LAB02_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>
#include <string>
#include <cmath>
#include <thread>
#include <algorithm>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

const float MAX = 1.0e9;

enum Movement
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    Z,
    X,
    W,
    A,
    S,
    D,
    Q,
    E
};

void printMat4(const glm::mat4 &mat)
{
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            std::cout << mat[i][j] << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

//TODO: 1. Game.h DONE
//      2. Scene.h DONE
//      3. Shadow.h HARD
//      4. Collision Detection HARD
//      5. Optimize 't' to change camera SEMI-DONE
//      6. Reshape function SEMI-DONE NEED CHECK
//      7. Menu CLOSED 'CAUSE GLFW DON'T SUPPORT
//      8. Refine all the thing

#endif //LAB02_UTILS_H
