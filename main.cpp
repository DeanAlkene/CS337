#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;
/*
const char* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aNormal;\n"
        "out vec3 Normal;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   Normal = aNormal;\n"
        "}\0";
const char* fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 Normal;\n"
        "out vec4 FragColor;\n"
        "uniform vec4 objColor;\n"
        "void main()\n"
        "{\n"
        "FragColor = objColor;\n"
        "}\n\0";
*/
vector<glm::vec3> vertices;
vector<glm::vec3> normals;
vector<glm::vec3> vertexAndNormal;
vector<GLuint> faces;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window); //check if keyboard input ESC in the rendering loop
void loadObj(const char* filename);
void calculateNormal();

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dragon", NULL, NULL);
    if(!window)
    {
        cerr << "Failed to create window!" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
/*------------------------------------------------------------------*/
    Shader shader("./lab01.vs", "./lab01.fs");
/*------------------------------------------------------------------*/
    loadObj("./dragon.obj");
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); //Create Vertex Buffer Object using Buffer ID = 1
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind VBO to Buffer Type
    glBufferData(GL_ARRAY_BUFFER, vertexAndNormal.size()*sizeof(vertexAndNormal[0]), vertexAndNormal.data(), GL_STATIC_DRAW); //Copy vertices to buffer memory use DYNAMIC/STREAM DRAW if it will change

    //Index Drawing Optional
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(faces[0]), faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); //How to explain vertex data
    glEnableVertexAttribArray(0);
/*------------------------------------------------------------------*/

    while(!glfwWindowShouldClose(window))
    {
        //Keyboard Input
        processInput(window);

        //Rendering Operations
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.activate();

        //Color changing respected to time, using uniform
        float timeValue = glfwGetTime();
        float blueValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue = sin(timeValue + glm::half_pi<float>()) / 2.0f + 0.5f;
        float greenValue = sin(timeValue + glm::pi<float>()) / 2.0f + 0.5f;
        glm::mat4 trans(1.0f);
        trans = glm::rotate(trans, timeValue, glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setVec4("objColor", redValue, greenValue, blueValue, 1.0f);
        shader.setMat4("transform", trans);

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        //Index Drawing Optional
        glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void loadObj(const char* filename)
{
    ifstream in;
    string line;
    in.open(filename);
    if(!in)
    {
        cerr << "Failed to open " << filename << endl;
        exit(1);
    }

    while(getline(in, line))
    {
        if(line.substr(0,2) == "v ")
        {
            istringstream is(line.substr(2));
            glm::vec3 v;
            is >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if(line.substr(0,2) == "f ")
        {
            istringstream is(line.substr(2));
            GLuint a, b, c;
            is >> a >> b >> c;
            a--;
            b--;
            c--;
            faces.push_back(a);
            faces.push_back(b);
            faces.push_back(c);
        }
    }
    calculateNormal();
    in.close();
}

void calculateNormal()
{
    normals.resize(vertices.size(), glm::vec3(0.0,0.0,0.0));
    //vector<glm::vec3> planeNormals;
    //planeNormals.resize(faces.size(), glm::vec3(0.0,0.0,0.0));
    for(auto i = 0; i < faces.size(); i += 3)
    {
        GLuint x, y, z;
        x = faces[i];
        y = faces[i+1];
        z = faces[i+2];
        glm::vec3 n = glm::normalize(
                glm::cross(
                        (vertices[y]-vertices[x]),
                        (vertices[z]-vertices[x])));
        //normals[x] = normals[y] = normals[z] = n;
        //}

        //Some problems
        normals[x] += n;
        normals[y] += n;
        normals[z] += n;
    }
    for(auto i = 0; i < vertices.size(); ++i)
    {
        normals[i] = glm::normalize(normals[i]);
    }
    for(auto i = 0; i < vertices.size(); ++i)
    {
        vertexAndNormal.push_back(vertices[i]);
        vertexAndNormal.push_back(normals[i]);
    }
}