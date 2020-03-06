#ifndef _CUBE_H_
#define _CUBE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "Object.h"

class Cube : public Object
{
private:
    GLuint vao;
    GLuint vbo;
    
    GLuint shaderProgram;
public:
	Cube(GLuint shaderProgram);
	~Cube();

	void draw();
	void update();

	void spin(float deg);
    unsigned int cubemapTexture;
    unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif

