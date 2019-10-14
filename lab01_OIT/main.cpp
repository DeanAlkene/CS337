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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window); //check if keyboard input ESC in the rendering loop
void oitInitializer(GLuint &list_build_program,
                    GLuint &head_pointer_texture,
                    GLuint &head_pointer_clear_buffer,
                    GLuint &atomic_counter_buffer,
                    GLuint &linked_list_buffer,
                    GLuint &linked_list_texture);

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

    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

/*------------------------------------------------------------------*/
    Shader renderShader("./vertex.glsl", "./fragment.glsl");
    Shader resolveShader("./resolve_vertex.glsl", "./resolve_fragment.glsl");
/*------------------------------------------------------------------*/
    GLuint  list_build_program;
    GLuint  head_pointer_texture;
    GLuint  head_pointer_clear_buffer;
    GLuint  atomic_counter_buffer;
    GLuint  linked_list_buffer;
    GLuint  linked_list_texture;

    oitInitializer(list_build_program,
            head_pointer_texture,
            head_pointer_clear_buffer,
            atomic_counter_buffer,
            linked_list_buffer,
            linked_list_texture);

    unsigned int VAO, VBO, EBO;
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

    glClearDepth(1.0f);
/*------------------------------------------------------------------*/

    while(!glfwWindowShouldClose(window))
    {
        //Keyboard Input
        processInput(window);

        /*----------------------WARNING---------------------------*/
        GLuint * data;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        /*----------------------WARNING---------------------------*/

        //Rendering Operations
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*----------------------WARNING---------------------------*/
        // Reset atomic counter
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_counter_buffer);
        data = (GLuint *)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_WRITE_ONLY);
        data[0] = 0;
        glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

        // Clear head-pointer image
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, head_pointer_clear_buffer);
        glBindTexture(GL_TEXTURE_2D, head_pointer_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);


        /*----------------------WARNING---------------------------*/

        renderShader.activate();

        //Color changing respected to time, using uniform
        float timeValue = glfwGetTime();
        float blueValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue = sin(timeValue + glm::three_over_two_pi<float>() / 2.0f) / 2.0f + 0.5f;
        float greenValue = sin(timeValue + glm::three_over_two_pi<float>()) / 2.0f + 0.5f;

        renderShader.setVec3("objColor", redValue, greenValue, blueValue);
        renderShader.setFloat("alpha", ALPHA);
        renderShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        renderShader.setVec3("lightPos", lightPos);
        renderShader.setVec3("viewPos", camera.getPosition());

        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;

        glm::mat4 model(1.0f);
        glm::mat4 view(1.0f);
        glm::mat4 projection(1.0f);

        model = obj.model;
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        renderShader.setMat4("model", model);
        renderShader.setMat4("view", view);
        renderShader.setMat4("projection", projection);

        /*----------------------WARNING---------------------------*/

        /*----------------------WARNING---------------------------*/

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, obj.faces.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        resolveShader.activate();

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, obj.faces.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // Bind head-pointer image for read-write
        glBindImageTexture(0, head_pointer_texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

        // Bind linked-list buffer for write
        glBindImageTexture(1, linked_list_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void oitInitializer(GLuint &list_build_program,
                    GLuint &head_pointer_texture,
                    GLuint &head_pointer_clear_buffer,
                    GLuint &atomic_counter_buffer,
                    GLuint &linked_list_buffer,
                    GLuint &linked_list_texture)
{
    GLuint * data;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &head_pointer_texture);
    glBindTexture(GL_TEXTURE_2D, head_pointer_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, MAX_FRAMEBUFFER_WIDTH, MAX_FRAMEBUFFER_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindImageTexture(0, head_pointer_texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI);

    // Create buffer for clearing the head pointer texture
    glGenBuffers(1, &head_pointer_clear_buffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, head_pointer_clear_buffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    data = (GLuint *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    memset(data, 0xFF, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * sizeof(GLuint));
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    // Create the atomic counter buffer
    glGenBuffers(1, &atomic_counter_buffer);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter_buffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_COPY);

    // Create the linked list storage buffer
    glGenBuffers(1, &linked_list_buffer);
    glBindBuffer(GL_TEXTURE_BUFFER, linked_list_buffer);
    glBufferData(GL_TEXTURE_BUFFER, MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT * 3 * sizeof(glm::vec4), NULL, GL_DYNAMIC_COPY);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    // Bind it to a texture (for use as a TBO)
    glGenTextures(1, &linked_list_texture);
    glBindTexture(GL_TEXTURE_BUFFER, linked_list_texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32UI, linked_list_buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    glBindImageTexture(1, linked_list_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);
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
        firstMouse = false;
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