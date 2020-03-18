#include "Scenery.h"
#include <string>
#include <string>
#include <sstream> 

#define TERRAIN_SIZE 500.0f

Scenery::Scenery(int width, int height, GLuint skybox_texture)
{
	this->width = width;
	this->height = height;
	this->boundaries.x = width * TERRAIN_SIZE;
	this->boundaries.y = height * TERRAIN_SIZE;
	this->skybox = skybox_texture;
	this->generateTerrains();
}

Scenery::~Scenery()
{
	for (Terrain * terrain : terrains) { delete(terrain); }
	for (Particle * particle : particles) { delete(particle); }
}

void Scenery::generateTerrains()
{
	terrains.clear();
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			Terrain* cur_terrain = new Terrain(j, i, "src/marslike01dn.ppm");
			terrains.push_back(cur_terrain);
		}
	}
}

void Scenery::draw_terrain(GLuint shaderProgram)
{
	for (int i = 0; i < terrains.size(); i++)
	{
		terrains[i]->draw(shaderProgram);
	}
}

int Scenery::getTerrain(glm::vec3 position)
{
	float position_x = position.x / TERRAIN_SIZE;
	float position_z = position.z / TERRAIN_SIZE;

	if (position_x >= width || position_x < 0 || position_z >= height || position_z < 0) {
		return 0;
	}

	int terrain_x = (int)floor(position_x);
	int terrain_z = (int)floor(position_z);

	return (terrain_z*width + terrain_x);
}

void Scenery::updateTerrainHeights() {
	for (Terrain* t : terrains) { t->updateHeights(); }
}

glm::vec2 Scenery::getBounds()
{
	glm::vec2 toReturn = this->boundaries;
	return toReturn;
}

