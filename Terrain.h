#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include "Window.h"
#include "Definitions.h"

class Terrain
{
private:
	GLuint terrainTexture;
	std::vector<Container> containers;//[v, vn, (s,t)]
	std::vector<glm::vec3> vertices;//v
	std::vector<glm::vec3> normals;//vn
	std::vector<glm::vec2> texCoords;//(s,t)
	std::vector<unsigned int> indices;//indices
	GLuint VAO, VBO, EBO;
	void setupHeightMap();
	float getHeightFromVertex(int x, int y);
	void updateNormals();
	unsigned char * loadPPM(const char* filename, int& width, int& height);
	GLuint loadTerrain(const char* filename, int index);
	void setupTerrain(const char* terrain);
	int draw_mode;

public:
	Terrain(int x_d, int z_d, const char* terrain);
	~Terrain();
	float x, z;
	glm::mat4 toWorld;
	void draw(GLuint shaderProgram);
	void update();
	void updateHeights();
};
#endif