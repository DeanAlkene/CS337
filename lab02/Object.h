//
// Created by dean on 19-10-9.
//

#ifndef LAB01_OBJECT_H
#define LAB01_OBJECT_H

#include "Utils.h"

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 800;

enum CLICK_STATUS
{
    LEFT_ONLY,
    RIGHT_ONLY,
    LEFT_AND_RIGHT
};

class Object
{
private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> vertexAndNormal;
    std::vector<GLuint> vertexIdx;
    std::vector<GLuint> uvIdx;
    std::vector<GLuint> normalIdx;
    std::map<GLuint, GLuint> verNorm;

    void loadObj(const char* filename)
    {
        std::ifstream in;
        std::string line;
        in.open(filename);
        if(!in)
        {
            std::cerr << "Failed to open " << filename << std::endl;
            exit(1);
        }

        while(getline(in, line))
        {
            if(line.substr(0,2) == "v ")
            {
                std::istringstream is(line.substr(2));
                glm::vec3 v;
                is >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if(line.substr(0,2) == "f ")
            {
                std::istringstream is(line.substr(2));
                std::string s_in[3];
                is >> s_in[0] >> s_in[1] >> s_in[2];
                std::vector<GLuint> tmpVec;
                tmpVec.clear();
                
                for(int i = 0; i < 3; ++i)
                {
                    std::stringstream input(s_in[i]);
                    std::string tmp;
                    while(getline(input, tmp, '/'))
                    {   
                        if(tmp != "")
                        {
                            GLuint tmpIdx = std::atoi(tmp.c_str());
                            tmpIdx--;
                            tmpVec.push_back(tmpIdx);
                        }
                    }
                }
                
                if(tmpVec.size() == 9)
                {
                    for(int i = 0; i < 9; i += 3)
                    {
                        vertexIdx.push_back(tmpVec[i]);
                        uvIdx.push_back(tmpVec[i+1]);
                        normalIdx.push_back(tmpVec[i+2]);
                        verNorm[tmpVec[i]] = tmpVec[i+2];
                    }
                }
                else if(tmpVec.size() == 6)
                {
                    for(int i = 0; i < 6; i += 2)
                    {
                        vertexIdx.push_back(tmpVec[i]);
                        normalIdx.push_back(tmpVec[i+1]);
                        verNorm[tmpVec[i]] = tmpVec[i+1];
                    }
                }
                else if(tmpVec.size() == 3)
                {
                    for(int i = 0; i < 3; i++)
                    {
                        vertexIdx.push_back(tmpVec[i]);
                    }
                }
                else
                {
                    std::cerr << "ERROR::FAILED TO READ .OBJ FILE." << std::endl;
                    exit(1);
                }
            }
            else if(line.substr(0,2) == "vn")
            {
                std::istringstream is(line.substr(3));
                glm::vec3 n;
                is >> n.x >> n.y >> n.z;
                normals.push_back(n);
            }
            else if(line.substr(0,2) == "vt")
            {
                std::istringstream is(line.substr(3));
                glm::vec2 t;
                is >> t.x >> t.y;
                uvs.push_back(t);
            }
        }
        in.close();
    }

    void calculateNormal()
    {
        for(auto it = verNorm.begin(); it != verNorm.end(); ++it)
        {
            vertexAndNormal.push_back(vertices[it->first]);
            vertexAndNormal.push_back(normals[it->second]);
        }
    }
    
    // void calculateNormal()
    // {
    //     normals.resize(vertices.size(), glm::vec3(0.0,0.0,0.0));
    //     vertexNormCount.resize(vertices.size(), 0);

    //     for(auto i = 0; i < vertexIdx.size(); i += 3)
    //     {
    //         GLuint x, y, z;
    //         x = vertexIdx[i];
    //         y = vertexIdx[i+1];
    //         z = vertexIdx[i+2];
    //         glm::vec3 n = glm::normalize(glm::cross((vertices[y]-vertices[x]),(vertices[z]-vertices[y])));
    //         normals[x] += n;
    //         normals[y] += n;
    //         normals[z] += n;
    //         vertexNormCount[x]++;
    //         vertexNormCount[y]++;
    //         vertexNormCount[z]++;
    //     }
    //     for(auto i = 0; i < vertices.size(); ++i)
    //     {
    //         normals[i] = glm::normalize(normals[i] / (float)vertexNormCount[i]);
    //     }
    //     for(auto i = 0; i < vertices.size(); ++i)
    //     {
    //         vertexAndNormal.push_back(vertices[i]);
    //         vertexAndNormal.push_back(normals[i]);
    //     }
    // }
    
    float getDeltaAngle(float xoffset, float yoffset)
    {
        float dist = glm::length(glm::vec2(xoffset, yoffset));
        float deltaAngle = fmodf(dist * 0.2, 360.0f);
        return deltaAngle;
    }
public:
    unsigned int VAO, VBO, EBO;
    glm::mat4 rot;
    glm::mat4 model;

    Object(const char* filename)
    {
        rot = glm::mat4(1.0f);
        model = glm::mat4(1.0f);
        loadObj(filename);
        calculateNormal();
    }

    void drawInit()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO); //Create Vertex Buffer Object using Buffer ID = 1
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind VBO to Buffer Type
        glBufferData(GL_ARRAY_BUFFER, vertexAndNormal.size()*sizeof(vertexAndNormal[0]), vertexAndNormal.data(), GL_STATIC_DRAW); //Copy vertices to buffer memory use DYNAMIC/STREAM DRAW if it will change

        //Index Drawing Optional
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIdx.size()*sizeof(vertexIdx[0]), vertexIdx.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0); //How to explain vertex data
        glEnableVertexAttribArray(0);
    
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    void draw()
    {
        glDrawElements(GL_TRIANGLES, vertexIdx.size(), GL_UNSIGNED_INT, 0);
    }

    void bindArray()
    {
        glBindVertexArray(VAO);
    }

    void unbindArray()
    {
        glBindVertexArray(0);
    }

    void processMouseDrag(CLICK_STATUS status, float xoffset, float yoffset, bool firstMouse)
    {
        if(firstMouse)
        {
            if(xoffset == 0.0f)
            {
                xoffset += 1e-9;
            }
            if(yoffset == 0.0f)
            {
                yoffset += 1e-9;
            }
        }
        
        if(status == LEFT_ONLY)
        {
            glm::vec3 dir = glm::vec3(xoffset / (float)WINDOW_WIDTH, yoffset / (float)WINDOW_HEIGHT, 0.0f);
            dir = glm::vec3(glm::inverse(rot) * glm::vec4(dir, 1.0f));
            model = glm::translate(model, dir);
        }
        else if(status == LEFT_AND_RIGHT)
        {
            glm::vec3 axis = glm::vec3(-yoffset, xoffset, 0.0f);
            float deltaAngle = getDeltaAngle(xoffset, yoffset);
            glm::mat4 deltaRot = glm::mat4(1.0f);
            deltaRot = glm::rotate(deltaRot, glm::radians(deltaAngle), axis);
            rot = deltaRot * rot;
            glm::vec3 actualAxis = glm::normalize(glm::vec3(glm::inverse(rot) * glm::vec4(axis, 1.0f)));
            model = glm::rotate(model, glm::radians(deltaAngle), actualAxis);
        }
    }
};

#endif //LAB01_OBJECT_H
