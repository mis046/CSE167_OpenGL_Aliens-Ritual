#include "Cube.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

Cube::Cube(GLuint shaderProgram)
{
    this->shaderProgram = shaderProgram;
    
    float vertices[] = {
        -1000.0f,  1000.0f, -1000.0f,
        -1000.0f, -1000.0f, -1000.0f,
         1000.0f, -1000.0f, -1000.0f,
         1000.0f, -1000.0f, -1000.0f,
         1000.0f,  1000.0f, -1000.0f,
        -1000.0f,  1000.0f, -1000.0f,

        -1000.0f, -1000.0f,  1000.0f,
        -1000.0f, -1000.0f, -1000.0f,
        -1000.0f,  1000.0f, -1000.0f,
        -1000.0f,  1000.0f, -1000.0f,
        -1000.0f,  1000.0f,  1000.0f,
        -1000.0f, -1000.0f,  1000.0f,

         1000.0f, -1000.0f, -1000.0f,
         1000.0f, -1000.0f,  1000.0f,
         1000.0f,  1000.0f,  1000.0f,
         1000.0f,  1000.0f,  1000.0f,
         1000.0f,  1000.0f, -1000.0f,
         1000.0f, -1000.0f, -1000.0f,

        -1000.0f, -1000.0f,  1000.0f,
        -1000.0f,  1000.0f,  1000.0f,
         1000.0f,  1000.0f,  1000.0f,
         1000.0f,  1000.0f,  1000.0f,
         1000.0f, -1000.0f,  1000.0f,
        -1000.0f, -1000.0f,  1000.0f,

        -1000.0f,  1000.0f, -1000.0f,
         1000.0f,  1000.0f, -1000.0f,
         1000.0f,  1000.0f,  1000.0f,
         1000.0f,  1000.0f,  1000.0f,
        -1000.0f,  1000.0f,  1000.0f,
        -1000.0f,  1000.0f, -1000.0f,

        -1000.0f, -1000.0f, -1000.0f,
        -1000.0f, -1000.0f,  1000.0f,
         1000.0f, -1000.0f, -1000.0f,
         1000.0f, -1000.0f, -1000.0f,
        -1000.0f, -1000.0f,  1000.0f,
         1000.0f, -1000.0f,  1000.0f
    };

    vector<std::string> faces;
    faces.push_back("src/right.jpg");
    faces.push_back("src/left.jpg");
    faces.push_back("src/top.jpg");
    faces.push_back("src/base.jpg");
    faces.push_back("src/front.jpg");
    faces.push_back("src/back.jpg");

    cubemapTexture = loadCubemap(faces);
    
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the vertices.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		vertices, GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access vertices through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO. We will use it to store the indices.
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[1]);
//	// Pass in the data.
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(),
//		indices.data(), GL_STATIC_DRAW);

	// Unbind from the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

Cube::~Cube()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(2, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Cube::draw()
{
//	// Bind to the VAO.
//	glBindVertexArray(vao);
//	// Draw triangles using the indices in the second VBO, which is an
//	// elemnt array buffer.
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
//	// Unbind from the VAO.
//	glBindVertexArray(0);
    
    glDepthMask(GL_FALSE);
    
    glUseProgram(shaderProgram);
    
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}

void Cube::update()
{
	// Spin the cube by 1 degree.
//	spin(0.01f);
}

void Cube::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(glm::mat4(1), glm::radians(deg), 
		glm::vec3(0.0f, 1000.0f, 0.0f)) * model;
}

unsigned int Cube::loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
 
    
    // Some settings
    // Make sure single-sided rendering (triangle culling) is enabled with these lines somewhere in your code to ensure that you will never see the outside of the box (this assumes that your sky box is defined with the triangles facing inward):
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Use the following settings for your texture after your first glBindTexture(GL_TEXTURE_CUBE_MAP, id) for correct lighting and filtering settings:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}  
