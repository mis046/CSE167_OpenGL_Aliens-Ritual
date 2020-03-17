//
//  Geometry.hpp
//  CSE167_hw3_new
//
//  Created by M Shen on 2/27/20.
//  Copyright © 2020 CSE167. All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Node.hpp"
#include "Material.h"

#include <unordered_map>

#include "Window.h"

using namespace std;

class Geometry : public Node
{
protected:
    glm::mat4 C;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> npoints;
    std::vector<unsigned int> faces;
    GLuint vao, vbo, ebo;
    // Used to normalize geomoetries
    glm::vec3 objCenterVec;
    float objScaleVal;
    GLuint * shaderProgram;
    
    
public:
    Material material;
    
    // set the modelview matrix to the current C matrix
    void setC(glm::mat4 C) {
        this->C = C;
    }
    
    void setMaterial(Material m) {
        this->material.ambient = m.ambient;
        this->material.diffuse = m.diffuse;
        this->material.specular = m.specular;
        this->material.shininess = m.shininess;
        this->material.color = m.color;
    }
    
    // an initialization method to load a 3D model (OBJ file) whose filename is passed to it (init(string filename). Your OBJ loader from project 2 should work.
    Geometry(std::string filename, GLuint * shaderProgram) {
        // Set model
        this->shaderProgram = shaderProgram;
        glUseProgram(*shaderProgram);

        C = glm::mat4(1.0f); // Initialize
        readPointsFromFile(filename, &points, &npoints, &faces);
        // VAO & VBO 1
        // Generate a vertex array (VAO) and a vertex buffer objects (VBO).
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        // Bind to the VAO.
        // This tells OpenGL which data it should be paying attention to
        glBindVertexArray(vao);
        // Bind to the first VBO. We will use it to store the points.
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Pass in the data.
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
         points.data(), GL_STATIC_DRAW);
        // Enable vertex attribute 0.
        // We will be able to access points through it.
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);
        // Unbind from the VBO.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // VBO 2
        glGenBuffers(1, &vbo);
        // Bind to the first VBO. We will use it to store the points.
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        // Pass in the data.
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * npoints.size(), npoints.data(), GL_STATIC_DRAW);
        // Enable vertex attribute 1.
        // We will be able to access npoints through it.
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        // Unbind from the VBO.
        glBindBuffer(GL_ARRAY_BUFFER, 1);
        // Create EBO
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        // Note: Second parameter is different because previous vbos we are using pointers, this time we use unsinged int
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(), faces.data(), GL_STATIC_DRAW);
        // Unbind from the VAO.
        glBindVertexArray(0);
    };
    
    ~Geometry() {
        // Delete the VBO and the VAO.
        // Failure to delete your VAOs, VBOs and other data given to OpenGL
        // is dangerous and may slow your program and cause memory leaks
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &ebo);
    }
    
    // have a class method which draws the 3D model associated with this node
    void draw(glm::mat4 C) {
        this->C = C;
        glUseProgram(*shaderProgram);
        
        // Pass in the material
        glUniform3fv(glGetUniformLocation(*shaderProgram, "material_ambient"), 1, glm::value_ptr(material.ambient));
        glUniform3fv(glGetUniformLocation(*shaderProgram, "material_diffuse"), 1, glm::value_ptr(material.diffuse));
        glUniform3fv(glGetUniformLocation(*shaderProgram, "material_specular"), 1, glm::value_ptr(material.specular));
        glUniform1f(glGetUniformLocation(*shaderProgram, "material_shininess"), material.shininess);
        glUniform3fv(glGetUniformLocation(*shaderProgram, "objectColor"), 1, glm::value_ptr(material.color));
        
        glUniformMatrix4fv(glGetUniformLocation(*shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(this->C));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void update() {
        
    }
    
    void readPointsFromFile(string path, vector<glm::vec3> *vPoints, vector<glm::vec3> *npoints, vector<unsigned int> *faces) {
        ifstream in(path);
        string str;
        float data[3];
        
        float maxX = -100000;
        float minX = 100000;
        float maxY = maxX;
        float minY = minX;
        float maxZ = maxX;
        float minZ = minX;
        
        vector<glm::vec3> temp_normals;
        unordered_map<unsigned int, unsigned int> vToN;

        while (getline(in, str)) {
            istringstream ss(str);
            string start;
            ss >> start;
            
            // Load the vertices
            if (start.compare("v") == 0) {
                for (int i = 0; i < 3; i++) { // Ignoring rgb right now
                    ss >> data[i];
                }
                glm::vec3 vecTemp = glm::vec3(data[0], data[1], data[2]);
                vPoints->push_back(vecTemp);
                
                // update max min
                maxX = std::max(vecTemp.x, maxX);
                minX = std::min(vecTemp.x, minX);
                maxY = std::max(vecTemp.y, maxY);
                minY = std::min(vecTemp.y, minY);
                maxZ = std::max(vecTemp.z, maxZ);
                minZ = std::min(vecTemp.z, minZ);
            }
            // Load the normal vertices into a different vector
            else if (start.compare("vn") == 0) {
                // Do things for vn
                for (int i = 0; i < 3; i++) { // Ignoring rgb right now
                    ss >> data[i];
                }
                temp_normals.push_back(glm::vec3(data[0], data[1], data[2]));
            }
            // Read in the connections of faces
            else if (start.compare("f") == 0) {
                string temp;
                // For example
                // f 7079/7929/7078 7078/7928/7077 7086/7941/7085 7087/7943/7086
                // 123 234
                unsigned int facePoints[4];
                for (int i = 0; i < 4; i ++) {
                    ss >> temp;
                    const char *c = temp.c_str();
                    unsigned int vIndex = std::stoi(c, NULL, 0) - 1; // Parse the string, IMPORTANT: 1 indexed!!!

                    unsigned long ptr = temp.find("/");
                    ptr = temp.find("/", ptr + 1);
                    unsigned int nIndex = std::stoi(temp.substr(ptr + 1), NULL, 0) - 1; // Parse the string, IMPORTANT: 1 indexed!!!

                    facePoints[i] = vIndex;
					vToN.insert({vIndex, nIndex});
                }
                for (int i = 0; i < 3; i++)
                    faces->push_back(facePoints[i]);
                for (int i = 1; i < 4; i++)
                    faces->push_back(facePoints[i]);
                faces->push_back(facePoints[2]);
                faces->push_back(facePoints[3]);
                faces->push_back(facePoints[0]);
                faces->push_back(facePoints[3]);
                faces->push_back(facePoints[0]);
                faces->push_back(facePoints[1]);
            }
        }

//        cout << vPoints->size() << "\n";
//        cout << vToN.size() << "\n";
        for (int i = 0; i < vPoints->size(); i++) {
            npoints->push_back(temp_normals[vToN.at(i)]);
        }
        
        // Set the center coordinate objCenterVec
        objCenterVec = glm::vec3((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);
        
        // Set the scale value to set all objects to same size
        // Points range [-1, 1]
        float maxCoord = std::max(std::max(abs(maxX - objCenterVec.x), abs(minX - objCenterVec.x)),
                             std::max(std::max(abs(maxY - objCenterVec.y), abs(minY - objCenterVec.y)),
                                 std::max(abs(maxZ - objCenterVec.z), abs(minZ - objCenterVec.z))));
        objScaleVal = 1.0f / maxCoord;
        
        
//        cout << "points: " << vPoints->size() << " normals: " << npoints->size() << "\n";
        in.close();
    }
};

#endif /* Geometry_h */
