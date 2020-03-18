#include "Terrain.h"
#include "HeightGenerator.h"
#include <time.h>
#include <math.h>
#include <ctime>

using namespace std;

#define SIZE 500
#define VERTEX_COUNT 128
#define MAX_HEIGHT 40
#define MAX_DISPLACEMENT 0.01f
#define DRAW_SHADED 0
#define SCENE_MODE 0

Terrain::Terrain(int x_d, int z_d, const char* terrain)
{
	this->x = x_d * SIZE;
	this->z = z_d * SIZE;
	this->draw_mode = DRAW_SHADED;
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;
	this->setupHeightMap();
	this->setupTerrain(terrain);
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Terrain::setupHeightMap()
{
	time_t currTime = time(nullptr);
	HeightGenerator hg = HeightGenerator(currTime % rand());
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			float vertex_x = ((float)j / ((float)VERTEX_COUNT - 1) * SIZE) - (SIZE / 2.0f);
			float vertex_z = ((float)i / ((float)VERTEX_COUNT - 1) * SIZE) - (SIZE / 2.0f);
			float vertex_y = hg.generateHeight(vertex_x / 4.0f, vertex_z / 4.0f) - 20.0f;
			float normal_x = 0;
			float normal_y = 1.0f;
			float normal_z = 0;
			float texCoord_x = (float)j / ((float)VERTEX_COUNT - 1);
			float texCoord_y = (float)i / ((float)VERTEX_COUNT - 1);
			vertices.push_back(glm::vec3(vertex_x, vertex_y, vertex_z));
			normals.push_back(glm::vec3(normal_x, normal_y, normal_z));
			texCoords.push_back(glm::vec2(texCoord_x, texCoord_y));
		}
	}
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	for (int i = 0; i < VERTEX_COUNT * VERTEX_COUNT; i++)
	{
		Container container;
		container.vertex = vertices[i];
		container.normal = normals[i];
		container.texCoord = texCoords[i];
		containers.push_back(container);
	}
}

float Terrain::getHeightFromVertex(int x, int y)
{
	if (x < 0 || x >= VERTEX_COUNT || y < 0 || y >= VERTEX_COUNT)
	{
		return 0;
	}
	return this->vertices[(y*VERTEX_COUNT) + x].y;
}

void Terrain::updateHeights() {
	time_t currTime = time(nullptr);
	HeightGenerator hg = HeightGenerator(currTime * 18403481 % 421321);
	cout << "generator seed = " << hg.getSeed() << endl;
	for (int i = 0; i < vertices.size(); i++) {
		float newHeight = hg.generateHeight(vertices[i].x, vertices[i].z);
		this->vertices[i].y = newHeight;
	}
	updateNormals();
}

void Terrain::updateNormals()
{
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			float heightL = getHeightFromVertex(j - 1, i);
			float heightR = getHeightFromVertex(j + 1, i);
			float heightD = getHeightFromVertex(j, i + 1);
			float heightU = getHeightFromVertex(j, i - 1);

			glm::vec3 normal = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightU - heightD));
			this->normals[(i*VERTEX_COUNT) + j] = normal;
			this->containers[(i*VERTEX_COUNT) + j].normal = normal;
		}
	}
}

unsigned char * Terrain::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}
	retval_fgets = fgets(buf[0], BUFSIZE, fp);
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	rawData = new unsigned char[width * height * 3];
	read = (unsigned int)fread(rawData, width * height * 3, 1, fp);
	fclose(fp);
	if (read != 1)
	{
		std::cerr << "error parsing ppm file, incomplete data" << std::endl;
		delete[] rawData;
		width = 0;
		height = 0;
		return NULL;
	}
	return rawData;
}

GLuint Terrain::loadTerrain(const char* filename, int index)
{
	GLuint textureID;
	int width, height;
	unsigned char * image;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	image = loadPPM(filename, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

void Terrain::setupTerrain(const char* terrain) {
	//Create buffers/arrays.
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(VAO); 

	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, this->containers.size() * sizeof(Container), &this->containers[0], GL_STATIC_DRAW); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);

	//Vertex Positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)0); 

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, texCoord));

	this->terrainTexture = loadTerrain(terrain, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindVertexArray(0); 
}

void Terrain::draw(GLuint shaderProgram)
{
	glm::mat4 MVP = Window::P * Window::V * this->toWorld;
	glm::mat4 model = this->toWorld;
	glm::mat4 view = glm::mat4(glm::mat3(Window::V));
	glm::mat4 projection = Window::P;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camera_pos.x, Window::camera_pos.y, Window::camera_pos.z);
	glUniform1i(glGetUniformLocation(shaderProgram, "toon_shade"), Window::toonShadingOn);
	if (draw_mode == DRAW_SHADED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glBindVertexArray(VAO);
	
	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, this->terrainTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "TerrainTexture"), 0);

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Terrain::update()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->containers.size() * sizeof(Container), &this->containers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
