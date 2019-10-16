#include "Utils.h"
#include "Shader.h"
#include "Camera.h"
#include "Object.h"

using namespace std;

float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;

Camera camera(glm::vec3(0.0f, -0.1f, 2.0f));

glm::vec3 lightPos(0.0f, 2.0f, 1.0f);

Object obj("./dragon.obj");
Object screen("./screen.obj");

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window); //check if keyboard input ESC in the rendering loop

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
/*------------------------------------------------------------------*/
    Shader shader("./vertex.glsl", "./fragment.glsl");
    Shader resolveShader("./resolve_vertex.glsl", "./resolve_fragment.glsl");
/*------------------------------------------------------------------*/
    unsigned int VAO, VBO, EBO, quadVAO, quadVBO, quadEBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); //Create Vertex Buffer Object using Buffer ID = 1
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind VBO to Buffer Type
    glBufferData(GL_ARRAY_BUFFER, obj.vertexAndNormal.size()*sizeof(obj.vertexAndNormal[0]), obj.vertexAndNormal.data(), GL_STATIC_DRAW); //Copy vertices to buffer memory use DYNAMIC/STREAM DRAW if it will change

    //Index Drawing Optional
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.faces.size()*sizeof(obj.faces[0]), obj.faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); //How to explain vertex data
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO); //Create Vertex Buffer Object using Buffer ID = 1
    glGenBuffers(1, &quadEBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO); //Bind VBO to Buffer Type
    glBufferData(GL_ARRAY_BUFFER, screen.vertices.size()*sizeof(screen.vertices[0]), screen.vertices.data(), GL_STATIC_DRAW); //Copy vertices to buffer memory use DYNAMIC/STREAM DRAW if it will change

    //Index Drawing Optional
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, screen.faces.size()*sizeof(screen.faces[0]), screen.faces.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); //How to explain vertex data
    glEnableVertexAttribArray(0);
/*------------------------------------------------------------------*/
    unsigned int texture, FBO;

    glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 
	             GL_RGBA32F,
				 WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							  GL_TEXTURE_2D, texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
/*------------------------------------------------------------------*/

    while(!glfwWindowShouldClose(window))
    {
        //Keyboard Input
        processInput(window);

        //Rendering Operations
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);

        shader.activate();
        //Color changing respected to time, using uniform
        float timeValue = glfwGetTime();
        float blueValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue = sin(timeValue + glm::three_over_two_pi<float>() / 2.0f) / 2.0f + 0.5f;
        float greenValue = sin(timeValue + glm::three_over_two_pi<float>()) / 2.0f + 0.5f;

        shader.setVec3("objColor", redValue, greenValue, blueValue);
        shader.setFloat("alpha", ALPHA);
        shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.getPosition());

        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;

        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);

        model = obj.model;
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, obj.faces.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        resolveShader.activate();
        
        resolveShader.setInt("screenTexture", 0);

        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawElements(GL_TRIANGLES, screen.faces.size(), GL_UNSIGNED_INT, 0);
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
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
        camera.processMouseMovement(xoffset, yoffset);
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
        obj.processMouseDrag(LEFT_ONLY, xoffset, yoffset, firstMouse);
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        obj.processMouseDrag(LEFT_AND_RIGHT, xoffset, yoffset, firstMouse);

    if(firstMouse)
    {
        firstMouse = false;
    }
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
    if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.processKeyboard(Z, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        camera.processKeyboard(X, deltaTime);
}