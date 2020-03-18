#pragma once
#ifndef SCENERY_H
#define SCENERY_H

#include "Window.h"
#include "Definitions.h"
#include "Terrain.h"
#include "Particle.h"

class Scenery
{
private:
	int width;
	int height;
	glm::vec2 boundaries;
	GLuint skybox;
	std::vector<Terrain*> terrains;
	std::vector<Particle*> particles;
	int getTerrain(glm::vec3 position);
public:
	void generateTerrains();
	Scenery(int width, int height, GLuint skybox_texture);
	~Scenery();

	glm::vec2 getBounds();

	void draw_terrain(GLuint shaderProgram);
	void updateTerrainHeights();
};
#endif