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

/* Texture Container to hold certain textures. */
struct Texture {
	GLuint id;
	std::string type;
};

/*
struct Material {//Setup the object's material.
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};
*/

struct DirLight {//Directional Light
	bool on;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight {//Point Light
	bool on;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float quadratic;
};

struct SpotLight {//Spot Light
	bool on;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float quadratic;
	glm::vec3 direction;
	float spotCutoff;
	float spotExponent;
};

struct Global_Camera {//Global Camera.
	glm::vec3 position;
	glm::vec3 lookat;
	glm::vec3 up;
	glm::vec3 direction;
	glm::vec3 right;
};

struct Particles_struct {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	GLfloat Life;

	Particles_struct() : Position(glm::vec3(0.0f, 0.0f, 0.0f)), Velocity(glm::vec3(0.0f, 0.0f, 0.0f)), Color(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)), Life(1.0f) {}
};


#endif
