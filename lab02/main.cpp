#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include "Skybox.h"
#include "Car.h"

using namespace std;

float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;
bool cameraType = false;

Camera camera[2];

glm::vec3 lightPos[2] = {glm::vec3(0.5f, 1.0f, 1.0f), glm::vec3(-0.5f, 1.0f, 1.0f)};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Car &car); //check if keyboard input ESC in the rendering loop
void changeView(float deltaTime);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Running Kart", NULL, NULL);
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

/*------------------------------------------------------------------*/
    Shader shader_static("./vertex.glsl", "./fragment.glsl");
    Shader shader_car("./vertex.glsl", "./fragment_NT.glsl");
    Shader shader_skybox("./vertex_skb.glsl", "./fragment_skb.glsl");
/*------------------------------------------------------------------*/
    Object road(std::string("/home/dean/CS337/Models/Scene/Roads/Roads.obj"));
    Car car(std::string("/home/dean/CS337/Models/Scene/Car/Car.obj"), 10.0, 2.0);
    Object lights(std::string("/home/dean/CS337/Models/Scene/Lights.obj"));
    Object stopSigns(std::string("/home/dean/CS337/Models/Scene/StopSign/StopSign.obj"));
    Object speedLimit(std::string("/home/dean/CS337/Models/Scene/SpeedLimit/SpeedLimit.obj"));
    Object trafficLight(std::string("/home/dean/CS337/Models/Scene/trafficlight/trafficlight.obj"));
    Object trees(std::string("/home/dean/CS337/Models/Scene/Palm/Palm.obj"));
    Skybox skybox(std::string("/home/dean/CS337/Models/Scene/Skybox"), std::string(".tga"));
/*------------------------------------------------------------------*/
    camera[0] = Camera(glm::vec3(0.0f, 10.0f, 10.0f));

    while(!glfwWindowShouldClose(window))
    {
        //Keyboard Input
        processInput(window, car);

        //Rendering Operations
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeValue = glfwGetTime();
        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;
        camera[1].updatePos(car.getCameraPos());
        camera[1].updateFront(car.getDir());

        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);
        /*------------------------------------------------------------------------------*/
        shader_static.activate();

        shader_static.setVec3("light[0].position", lightPos[0]);
        shader_static.setVec3("light[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader_static.setVec3("light[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_static.setVec3("light[0].specular", glm::vec3(0.3f, 0.3f, 0.3f));
        shader_static.setVec3("light[1].position", lightPos[1]);
        shader_static.setVec3("light[1].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader_static.setVec3("light[1].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_static.setVec3("light[1].specular", glm::vec3(0.3f, 0.3f, 0.3f));
        shader_static.setVec3("viewPos", camera[cameraType].getPosition());
        shader_static.setFloat("alpha", ALPHA);

        view = camera[cameraType].getViewMatrix();
        projection = glm::perspective(glm::radians(camera[cameraType].getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);
        shader_static.setMat4("model", model);
        shader_static.setMat4("view", view);
        shader_static.setMat4("projection", projection);

        lights.Draw(shader_static);
        stopSigns.Draw(shader_static);
        speedLimit.Draw(shader_static);
        trafficLight.Draw(shader_static);
        road.Draw(shader_static);
        trees.Draw(shader_static);

        shader_static.deactivate();
        /*------------------------------------------------------------------------------*/

        /*------------------------------------------------------------------------------*/
        shader_car.activate();

        shader_car.setVec3("light[0].position", lightPos[0]);
        shader_car.setVec3("light[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader_car.setVec3("light[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_car.setVec3("light[0].specular", glm::vec3(0.3f, 0.3f, 0.3f));
        shader_car.setVec3("light[1].position", lightPos[1]);
        shader_car.setVec3("light[1].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader_car.setVec3("light[1].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader_car.setVec3("light[1].specular", glm::vec3(0.3f, 0.3f, 0.3f));
        shader_car.setVec3("viewPos", camera[cameraType].getPosition());
        shader_car.setFloat("alpha", ALPHA);

        model = car.getModel();
        view = camera[cameraType].getViewMatrix();
        projection = glm::perspective(glm::radians(camera[cameraType].getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);
        shader_car.setMat4("model", model);
        shader_car.setMat4("view", view);
        shader_car.setMat4("projection", projection);

        car.Draw(shader_car);

        shader_car.deactivate();
        /*------------------------------------------------------------------------------*/

        /*------------------------------------------------------------------------------*/
        glDepthFunc(GL_LEQUAL);
        shader_skybox.activate();
        view = glm::mat4(glm::mat3(camera[cameraType].getViewMatrix()));
        shader_skybox.setMat4("view", view);
        shader_skybox.setMat4("projection", projection);

        skybox.Draw(shader_skybox);
        glDepthFunc(GL_LESS);
        /*------------------------------------------------------------------------------*/
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
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
        camera[cameraType].processMouseMovement(xoffset, yoffset);

    if(firstMouse)
    {
        firstMouse = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera[cameraType].processMouseScroll(yoffset);
}

void processInput(GLFWwindow* window, Car &car)
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
        car.processKeyboard(W, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        car.processKeyboard(A, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        car.processKeyboard(S, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        car.processKeyboard(D, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        car.processKeyboard(Q, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        car.processKeyboard(E, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        changeView(deltaTime);
}

void changeView(float deltaTime)
{
    this_thread::sleep_for(chrono::milliseconds(300));
    cameraType = !cameraType;
}