//
//  Transform.hpp
//  CSE167_hw3_new
//
//  Created by M Shen on 2/27/20.
//  Copyright © 2020 CSE167. All rights reserved.
//

#ifndef Transform_h
#define Transform_h

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include "Node.hpp"

class Transform : public Node
{
protected:
    // store a 4x4 transformation matrix M
    glm::mat4 M;
    // store a list of pointers to child nodes (std::vector<Node*>)
    std::vector<Node*> childNodes;
    bool moveF = true;
    float moved = 0.0f;

    float deg = 0.08f;
public:
    
    // provide a class methods to add a child node (addChild(), removeChild()) to the list
    Transform(glm::mat4 M) {
        this->M = M;
    };
    
    ~Transform() {
        
    };
    
    void addChild(Node* n) {
        childNodes.push_back(n);
    };
    void removeChild(Node* n) {
        for (int i = 0; i < childNodes.size(); i++) {
            if (childNodes.at(i) == n) {
                // vector.erase( vector.begin() + 3 ); // Deleting the fourth element
                childNodes.erase(childNodes.begin() + i);
            }
        }
    };
    // its draw method needs to traverse the list of children and call each child node's draw function
    // when draw(C) is called, multiply matrix M with matrix C.
    void draw(glm::mat4 C) {
        glm::mat4 MC =  C * M;
        for (int i = 0; i < childNodes.size(); i++) {
            childNodes.at(i)->draw(MC);
        }
    };
    
    void update() {
        
    }
    
    void moveTo(glm::vec3 point) {
        M = translate(M, (point - getPos()));
    }
    
    void rotate(glm::vec3 axis, float deg) {
        M = glm::translate(M, getPos());
        M = glm::rotate(M, deg, axis);
        //        scaleFactor *= s;
        M = glm::translate(M, getPos());
    }
    
    
    void moveL() {
        if (moved > 0.8 || moved < - 0.8) {
            moveF = !moveF;
        }
        if (moveF) {
            M = glm::scale(M, glm::vec3(1.005, 1.0, 1.001));
            moved += deg;
        }
        else {
            M = glm::scale(M, glm::vec3(1.0/1.005, 1.0, 1.0/1.001));
            moved -= deg;
        }
    }
    void moveR() {
        if (moved > 0.8 || moved < - 0.8) {
            moveF = !moveF;
        }
        if (moveF) {
            M = glm::rotate(M, -deg, glm::vec3(1.0, 0.0, 0.0));
            moved += deg;
        }
        else {
            M = glm::rotate(M, deg, glm::vec3(1.0, 0.0, 0.0));
            moved -= deg;
        }
    }
    
    glm::mat4 getM() {
        return M;
    }
    void setM(glm::mat4 M) {
        this->M = M;
    }
    glm::vec3 getPos() {
        return glm::vec3(M[3]);
    }
    
    void scaleUp() {
        M = glm::translate(M, getPos());
        M = glm::scale(M, glm::vec3(1.25));
//        scaleFactor *= s;
        M = glm::translate(M, getPos());
//        M = glm::scale(glm::vec3(1.1));
    }
    
    void scaleDown() {
        M = glm::translate(M, getPos());
        M = glm::scale(M, glm::vec3(0.8));
//        scaleFactor *= s;
        M = glm::translate(M, getPos());
    }
    
    void scale(float s) {
        M = glm::translate(M, getPos());
        M = glm::scale(M, glm::vec3(s));
//        scaleFactor *= s;
        M = glm::translate(M, getPos());
    }
    
};


#endif /* Transform_h */
