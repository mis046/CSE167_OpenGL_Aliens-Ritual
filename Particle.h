#ifndef _Particle_H_
#define _Particle_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class Particle
{
private:
	GLuint vao;
	GLuint vbos[2];
    GLuint shader;
    glm::mat4 model;
    glm::vec3 color;
    
    glm::vec3 displacement;
    
    float gravity = 0.0001;
public:
	Particle(float size, GLuint shader, int lifeLeft, glm::vec3 velocity);
	~Particle();
    
    glm::vec3 velocity;
    int lifeLeft;

    void resetPosition();
    
    glm::mat4 getModel() { return model; }
    glm::vec3 getColor() { return color; }
    
	void draw(glm::mat4 projection, glm::mat4 view);
	void update();

	void spin(float deg);
};

#endif

