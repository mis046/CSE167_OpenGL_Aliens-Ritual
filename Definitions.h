#pragma once

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* Container: [X, Y, Z] [R, G, B] [S, T] */
struct Container {
	//Position
	glm::vec3 vertex;
	//Normal
	glm::vec3 normal;
	//TexCoords
	glm::vec2 texCoord;
};

#endif
