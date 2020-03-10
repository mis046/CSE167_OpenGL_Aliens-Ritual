//
//  DirectionalLight.cpp
//  CSE167_hw2
//
//  Created by M Shen on 2/14/20.
//  Copyright © 2020 CSE167. All rights reserved.
//

#include "DirectionalLight.hpp"

using namespace glm;
using namespace std;

DirectionalLight::DirectionalLight(glm::vec3 d, glm::vec3 c)
{
    direction = d;
    color = c;
    angle = 0.0f;
}

void DirectionalLight::move() {
    angle += 0.001f;
    direction.x = 10.0f * sin(angle);
    direction.y = 10.0f * abs(sin(abs(360 - angle)));
}
