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
	//Contain the width and height of the scenery.
	int width;
	int height;
	glm::vec2 boundaries;
	GLuint skybox;
	//Access elements from the scenery class.
	std::vector<Terrain*> terrains;
	std::vector<Particle*> particles;
	//Terrains
	void generateTerrains();
	void stitchTerrains();
	int getTerrain(glm::vec3 position);
	//Water

public:
	//Constructor methods.
	Scenery(int width, int height, GLuint skybox_texture);
	~Scenery();

	float getHeight(glm::vec3 position);
	glm::vec2 getBounds();

	void toggleDrawMode();

	void draw_terrain(GLuint shaderProgram);
	void updateTerrainHeights();
};
#endif