#ifndef _NODE_H_
#define _NODE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

class Node
{
protected:
public:
    virtual void draw(glm::mat4 C) = 0;
    virtual void update() = 0;
    
    virtual ~Node() {}
};

#endif

