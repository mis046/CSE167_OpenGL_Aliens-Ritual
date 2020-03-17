//
//  BezierCurver.hpp
//  CSE167_hw3_new
//
//  Created by M Shen on 2/27/20.
//  Copyright © 2020 CSE167. All rights reserved.
//

#ifndef BezierCurver_h
#define BezierCurver_h

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

using namespace std;
using namespace glm;

class BezierCurve : public Node
{
protected:
    glm::mat4 C;
    vec3 color;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> npoints;
    std::vector<unsigned int> faces;
    GLuint vao, vbo, ebo;
    // Used to normalize geomoetries
    glm::vec3 objCenterVec;
    float objScaleVal;
    GLuint * shaderProgram;
    vector<vec3> controlPoints;
    vector<vec3> k;
    
public:
    // set the modelview matrix to the current C matrix
    void setC(glm::mat4 C) {
        this->C = C;
    }
    
    std::vector<vec3> getPoints() {
        return points;
    }
    
    // an initialization method to load a 3D model (OBJ file) whose filename is passed to it (init(string filename). Your OBJ loader from project 2 should work.
    BezierCurve(vector<vec3> controlPoints, GLuint * shaderProgram) {
        this->C = mat4(1.0f);
        this->color = vec3(1.0f);
        
        // Set model
        this->controlPoints = controlPoints;
        this->shaderProgram = shaderProgram;
        glUseProgram(*shaderProgram);

        // Calculate the p s
        vec3 p0 = controlPoints.at(0);
        vec3 p1 = controlPoints.at(1);
        vec3 p2 = controlPoints.at(2);
        vec3 p3 = controlPoints.at(3);

        k.push_back(-p0 + 3.0f * p1 - 3.0f * p2 + p3);
        k.push_back(3.0f * p0 - 6.0f * p1 + 3.0f * p2);
        k.push_back(-3.0f * p0 + 3.0f * p1);
        k.push_back(p0);

        // Produce a bunch of points
        // 200 points
        for (float t = 0.0f; t < 1.0f; t += 0.0005f) {
            points.push_back(generatePoint(t));
        }

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

       
    };
    
    glm::vec3 generatePoint(float t)
    {
        return k.at(0) * pow(t, 3.0f) + k.at(1) * pow(t, 2.0f) + k.at(2) * t + k.at(3) * 1.0f;
    }
    
    ~BezierCurve() {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    
    // have a class method which draws the 3D model associated with this node
    void draw(glm::mat4 C) {
        this->C = C;
        glUseProgram(*shaderProgram);
        GLuint modelLoc = glGetUniformLocation(*shaderProgram, "model");
        GLuint colorLoc = glGetUniformLocation(*shaderProgram, "color");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->C));
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        
        glBindVertexArray(vao);
        glPointSize(10);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());
        glBindVertexArray(0);
    };
    
    void update() {
        
    };
    
};

#endif /* BezierCurver_h */
