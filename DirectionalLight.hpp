//
//  DirectionalLight.hpp
//  CSE167_hw2
//
//  Created by M Shen on 2/14/20.
//  Copyright © 2020 CSE167. All rights reserved.
//

#ifndef DirectionalLight_hpp
#define DirectionalLight_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>

#include <stdio.h>

class DirectionalLight
{
public:
    glm::vec3 direction;
    glm::vec3 color;
    
    DirectionalLight(glm::vec3 d, glm::vec3 c);
    
    void move();
    float angle;
};

#endif /* DirectionalLight_hpp */
