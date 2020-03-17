//
//  Water.cpp
//  Project1
//
//  Created by Parth Patel on 5/27/16.
//  Copyright © 2016 Parth Patel. All rights reserved.
//

#include <stdio.h>
#include "Water.h"

#define HEIGHT 3
#define SIZE 500
#define WATER_SIZE 150
#define DRAW_SHADED 0
#define DRAW_WIREFRAME 1

/// 4x4 grid of points that will define the surface
Point Points[4][4] = {
    {
        { (2 * WATER_SIZE), HEIGHT, (2 * WATER_SIZE) },
		{ WATER_SIZE, HEIGHT, (2 * WATER_SIZE) },
		{ -WATER_SIZE, HEIGHT, (2 * WATER_SIZE) },
        {-(2*WATER_SIZE), HEIGHT, (2*WATER_SIZE) }
    },
    {
        { (2*WATER_SIZE), HEIGHT, WATER_SIZE },
        {  WATER_SIZE, HEIGHT, WATER_SIZE },
        { -WATER_SIZE, HEIGHT, WATER_SIZE },
        {-(2*WATER_SIZE), HEIGHT, WATER_SIZE }
    },
    {
        { (2*WATER_SIZE), HEIGHT, -WATER_SIZE },
        {  WATER_SIZE, HEIGHT, -WATER_SIZE },
        { -WATER_SIZE, HEIGHT, -WATER_SIZE },
        {-(2*WATER_SIZE), HEIGHT, -WATER_SIZE }
    },
    {
        { (2*WATER_SIZE), HEIGHT,-(2*WATER_SIZE) },
        {  WATER_SIZE, HEIGHT,-(2*WATER_SIZE) },
        { -WATER_SIZE, HEIGHT,-(2*WATER_SIZE) },
        {-(2*WATER_SIZE), HEIGHT, -(2*WATER_SIZE) }
    }
};

Water::Water(int x_d, int z_d)
{
	//Setup the Water.
	this->x = x_d * SIZE + (SIZE / 2);
	this->z = z_d * SIZE + (SIZE / 2);
	this->draw_mode = DRAW_SHADED;
	this->level_of_detail = 300;//Define how many vertices (width/depth).
	//Setup toWorld so that the terrain is at the center of the world.
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;
	//Setup the water surface.
	this->setupGeometry();
	this->setupWater();
}

Water::Water(int x_d, int z_d, GLuint skyBox_texture)
{
	//Setup the Water.
	this->x = x_d * SIZE + (SIZE / 2);
	this->z = z_d * SIZE + (SIZE / 2);
	this->draw_mode = DRAW_SHADED;
	this->level_of_detail = 300;//Define how many vertices (width/depth).
	this->skyTexture = skyBox_texture;
	//Setup toWorld so that the terrain is at the center of the world.
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;
	//Setup the water surface.
	this->setupGeometry();
	this->setupWater();
}

/* Deconstructor to safely delete when finished. */
Water::~Water()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBONORM);
	glDeleteBuffers(1, &EBO);
}

/* Setup the Bezier surfaces. */
void Water::setupGeometry()
{
	/* Setting up verticies and normals. */
	for (unsigned int i = 0; i != this->level_of_detail; ++i) {
		//Generate the parametric u value.
		float u = (float)i / (this->level_of_detail - 1);
		//Loop for the parametric v value, per section.
		for (unsigned int j = 0; j != this->level_of_detail; ++j) {
			//Generate the parametric v value.
			float v = (float)j / (this->level_of_detail - 1);
			//Calculate the point on the surface
			Point p = Calculate(u, v);
			//Add point into vertices and normals list.
			this->vertices.push_back(glm::vec3(p.x, p.y, p.z));
			this->normals.push_back((glm::vec3(0.0, 1.0, 0.0)));
		}
	}
	/* Setting up indices. */
	for (unsigned int gx = 0; gx < this->level_of_detail - 1; gx++)
	{
		for (unsigned int gz = 0; gz < this->level_of_detail - 1; gz++)
		{
			int topLeft = (gx*this->level_of_detail) + gz;
			int topRight = ((gx + 1)*this->level_of_detail) + gz;
			int bottomLeft = topLeft + 1;
			int bottomRight = topRight + 1;
			//Push back to indices.
			this->indices.push_back(topLeft);
			this->indices.push_back(bottomLeft);
			this->indices.push_back(topRight);
			this->indices.push_back(topRight);
			this->indices.push_back(bottomLeft);
			this->indices.push_back(bottomRight);
		}
	}
}

/* Calculate the first half of linear interpolation. */
Point Water::CalculateU(float t, int row)
{
	Point p;
	//The t value inverted.
	float it = 1.0f - t;
	//Calculate the blending functions.
	float b0 = t*t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it*it*it;
	//Sum the effects of the Points and their respective blending functions.
	p.x = b0*Points[row][0].x +
		b1*Points[row][1].x +
		b2*Points[row][2].x +
		b3*Points[row][3].x;

	p.y = b0*Points[row][0].y +
		b1*Points[row][1].y +
		b2*Points[row][2].y +
		b3*Points[row][3].y;

	p.z = b0*Points[row][0].z +
		b1*Points[row][1].z +
		b2*Points[row][2].z +
		b3*Points[row][3].z;
	//Return the adjusted point.
	return p;
}

/* Calculate the second half of linear interpolation. */
Point Water::CalculateV(float t, Point* pnts)
{
	Point p;
	//The t value inverted.
	float it = 1.0f - t;
	//Calculate the blending functions.
	float b0 = t*t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it*it*it;
	//Sum the effects of the Points and their respective blending functions.
	p.x = b0*pnts[0].x +
		b1*pnts[1].x +
		b2*pnts[2].x +
		b3*pnts[3].x;

	p.y = b0*pnts[0].y +
		b1*pnts[1].y +
		b2*pnts[2].y +
		b3*pnts[3].y;

	p.z = b0*pnts[0].z +
		b1*pnts[1].z +
		b2*pnts[2].z +
		b3*pnts[3].z;
	//Return the adjusted point.
	return p;
}

/* Perform Bezier Surface calculation. */
Point Water::Calculate(float u, float v)
{
	//First, evaluate 4 curves in the 'u' direction. The points will be stored in a temporary array.
	Point temp[4];
	//Calculate for each point on our final v curve.
	temp[0] = CalculateU(u, 0);
	temp[1] = CalculateU(u, 1);
	temp[2] = CalculateU(u, 2);
	temp[3] = CalculateU(u, 3);
	//We can use the results as a bezier curve to calculate the v direction. This should give us our final point.
	return CalculateV(v, temp);
}

/* Setup water for glsl. */
void Water::setupWater()
{
	//Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBONORM);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
		GL_FLOAT, // What type these components are
		GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
		3 * sizeof(GLfloat), // Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
		(GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBONORM);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

/* Toggle the draw mode to draw the mesh as lines (wireframe) or as triangle faces. */
void Water::toggleDrawMode()
{
	if (draw_mode == 0)
	{
		draw_mode = DRAW_WIREFRAME;
	}
	else
	{
		draw_mode = DRAW_SHADED;
	}
}

/* Draw the water mesh. */
void Water::draw(GLuint shaderProgram)
{
	//Get the current time.
    float time = (float)glfwGetTime();
    //Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices.
    glm::mat4 MVP = Window::P * Window::V * toWorld;
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &toWorld[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camera_pos.x, Window::camera_pos.y, Window::camera_pos.z);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
	//Update toon_shade.
	glUniform1i(glGetUniformLocation(shaderProgram, "toon_shade"), Window::toonShadingOn);
	//Set draw_mode to view wireframe version or filled version.
	if (draw_mode == DRAW_SHADED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (draw_mode == DRAW_WIREFRAME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
    //Bind for drawing.
    glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
    glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
	//Set it back to fill.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
