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
#include "Camera.h"
using namespace std;

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, -0.1f, 2.0f));

vector<glm::vec3> vertices;
vector<glm::vec3> normals;
vector<glm::vec3> vertexAndNormal;
vector<GLuint> faces;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();

        //Color changing respected to time, using uniform
        float timeValue = glfwGetTime();
        float blueValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue = sin(timeValue + glm::three_over_two_pi<float>() / 2.0f) / 2.0f + 0.5f;
        float greenValue = sin(timeValue + glm::three_over_two_pi<float>()) / 2.0f + 0.5f;
        shader.setVec4("objColor", redValue, greenValue, blueValue, 1.0f);

        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;

        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
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