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
#define DRAW_WIREFRAME 1
#define SCENE_MODE 0

Terrain::Terrain(int x_d, int z_d, const char* terrain)
{
	//Setup the terrain.
	this->x = x_d * SIZE;
	this->z = z_d * SIZE;
	this->draw_mode = DRAW_SHADED;
	//Setup toWorld so that the terrain is at the center of the world.
	this->toWorld = glm::mat4(1.0f);
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(this->x, 0, this->z));
	this->toWorld = translate*this->toWorld;
	//Setup HeightMap
	//this->setupHeightMap(height_map, 16.0f, 4.0f);
	this->setupHeightMap();
	//Load the texture and setup VAO, VBO for the terrains.
	this->setupTerrain(terrain);
}

/* Deconstructor to safely delete when finished. */
Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/* Setup a default flat terrain. */
void Terrain::setupHeightMap()
{
	time_t currTime = time(nullptr);
	HeightGenerator hg = HeightGenerator(currTime % 421321);
	//cout << hg.getSeed() << endl;
	//Create the height map: v, vn, texCoords
	//Generate vertices, normals, and texCoords for a terrain map. vertex = (j, i).
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//Setup the vertices.
			float vertex_x = ((float)j / ((float)VERTEX_COUNT - 1) * SIZE) - (SIZE / 2.0f);
			float vertex_z = ((float)i / ((float)VERTEX_COUNT - 1) * SIZE) - (SIZE / 2.0f);
			float vertex_y = hg.generateHeight(vertex_x / 4.0f, vertex_z / 4.0f) - 20.0f;
			//float vertex_y = getHeight(vec3(vertex_x, 0.0f, vertex_z));
			//float vertex_y = 0.0f;
			//cout << vertex_y << endl;
			//Setup the normals.
			float normal_x = 0;
			float normal_y = 1.0f;
			float normal_z = 0;
			//Setup the texcoords.
			float texCoord_x = (float)j / ((float)VERTEX_COUNT - 1);
			float texCoord_y = (float)i / ((float)VERTEX_COUNT - 1);
			//Push back to vectors.
			vertices.push_back(glm::vec3(vertex_x, vertex_y, vertex_z));
			normals.push_back(glm::vec3(normal_x, normal_y, normal_z));
			texCoords.push_back(glm::vec2(texCoord_x, texCoord_y));
		}
	}
	//Setup the indices to draw based on indice points.
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz*VERTEX_COUNT) + gx;//0//1
			int topRight = topLeft + 1;//1//2
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;//128//129
			int bottomRight = bottomLeft + 1;//129//130
			//Push back to indices.
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	//Add into container structs for rendering.
	for (int i = 0; i < VERTEX_COUNT * VERTEX_COUNT; i++)
	{
		//Throw everything into a container to hold all values.
		Container container;
		container.vertex = vertices[i];
		container.normal = normals[i];
		container.texCoord = texCoords[i];
		containers.push_back(container);
	}
}

/* Setup the terrain based on loaded height map. */
void Terrain::setupHeightMap(const char* filename, float n_smooth, float n_range)
{
	//Define variables to hold height map's width, height, pixel information.
	int width, height;
	unsigned char * image;
	//Generate the texture.
	image = loadPPM(filename, width, height);//Load the ppm file.
	//Create the height map: v, vn, texCoords
	//Generate vertices, normals, and texCoords for a terrain map.
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//Setup the vertices.
			float vertex_x = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
			float vertex_y = getHeightFromMap(j, i, image, width, height);
			float vertex_z = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
			//Setup the normals.
			float normal_x = 0;
			float normal_y = 1.0f;
			float normal_z = 0;
			//Setup the texcoords.
			float texCoord_x = (float)j / ((float)VERTEX_COUNT - 1);
			float texCoord_y = (float)i / ((float)VERTEX_COUNT - 1);
			//Push back to vectors.
			vertices.push_back(glm::vec3(vertex_x, vertex_y, vertex_z));
			normals.push_back(glm::vec3(normal_x, normal_y, normal_z));
			texCoords.push_back(glm::vec2(texCoord_x, texCoord_y));
		}
	}
	//Setup the indices to draw based on indice points.
	for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
	{
		for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
		{
			int topLeft = (gz*VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft = ((gz + 1)*VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;
			//Push back to indices.
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}
	//Add into container structs for rendering.
	for (int i = 0; i < VERTEX_COUNT * VERTEX_COUNT; i++)
	{
		//Throw everything into a container to hold all values.
		Container container;
		container.vertex = vertices[i];
		container.normal = normals[i];
		container.texCoord = texCoords[i];
		containers.push_back(container);
	}
	//Perform smoothing.
	diamond_square(0, VERTEX_COUNT-1, 0, VERTEX_COUNT-1, (int)glm::pow(2, n_smooth), (float)n_range);
	//Update normals and calculate max/min height.
	updateNormals();
	cout << "\t\t\tREACHED #3 Terrain" << endl;
	//updateMaxMinHeight();
	cout << "\t\t\tREACHED #4 Terrain" << endl;
}

/* Returns the RGB value of the position (height). */
float Terrain::getHeightFromMap(int x, int y, unsigned char * image, int width, int height)
{
	if (x < 0 || (3*x) >= (width) || y < 0 || y >= (height))
	{
		return 0;
	}
	int index = (3*x) + (width*y);
	float value = (float)image[index]+(float)image[index + 1]+(float)image[index + 2];
	float result = (float)fmod(value, (float)MAX_HEIGHT);
	return result;
}

/* Return the height at a given x, y coordinate. */
float Terrain::getHeightFromVertex(int x, int y)
{
	if (x < 0 || x >= VERTEX_COUNT || y < 0 || y >= VERTEX_COUNT)
	{
		return 0;
	}
	return this->vertices[(y*VERTEX_COUNT) + x].y;
}

/* Perform the diamond square algorithm at most 2^n steps. Pass in input level: 2^n, and small number for the range. */
void Terrain::diamond_square(int x1, int x2, int y1, int y2, int level, float range)
{
	//Check base case to stop recursion.
	if (level < 1)
		return;
	//Start random number generation.
	srand((unsigned int)time(NULL));
	//Diamond Algorithm
	for (int i = x1 + level; i < x2; i += level)
	{
		for (int j = y1 + level; j < y2; j += level)
		{
			//Get the 4 main vertices.
			glm::vec3 vec_a = this->vertices[(j - level)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_b = this->vertices[(j - level)*VERTEX_COUNT + i];
			glm::vec3 vec_c = this->vertices[(j)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_d = this->vertices[(j)*VERTEX_COUNT + i];
			//Get the middle vertex.
			glm::vec3 vec_e = this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level / 2)];
			//Get the 4 heights.
			float height_a = vec_a.y;
			float height_b = vec_b.y;
			float height_c = vec_c.y;
			float height_d = vec_d.y;
			//Calculate the average height in the middle and set it to E.
			float height_e = (float)(height_a + height_b + height_c + height_d) / 4;
			height_e += fmod(((float)(rand()) / 1000), MAX_DISPLACEMENT)*range;
			vec_e.y = height_e;
			this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level / 2)] = vec_e;
			this->containers[(j - level / 2)*VERTEX_COUNT + (i - level / 2)].vertex = vec_e;
		}
	}
	//Square algorithm
	for (int i = x1 + 2 * level; i < x2; i += level)
	{
		for (int j = y1 + 2 * level; j < y2; j += level)
		{
			//Get the 4 main vertices.
			glm::vec3 vec_a = this->vertices[(j - level)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_b = this->vertices[(j - level)*VERTEX_COUNT + i];
			glm::vec3 vec_c = this->vertices[(j)*VERTEX_COUNT + (i - level)];
			glm::vec3 vec_d = this->vertices[(j)*VERTEX_COUNT + i];
			//Get the middle vertex.
			glm::vec3 vec_e = this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level / 2)];
			//Get the 5 heights.
			float height_a = vec_a.y;
			float height_b = vec_b.y;
			float height_c = vec_c.y;
			float height_d = vec_d.y;
			float height_e = vec_e.y;
			//Calculate the average height and set it to F.
			glm::vec3 vec_f = this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level)];//
			float height_f = (float)(height_a + height_c + height_e + this->vertices[(j - level / 2)*VERTEX_COUNT + (i - 3 * level / 2)].y) / 3;
			height_f += fmod(((float)(rand()) / 1000), MAX_DISPLACEMENT)*range;
			vec_f.y = height_f;
			this->vertices[(j - level / 2)*VERTEX_COUNT + (i - level)] = vec_f;
			this->containers[(j - level / 2)*VERTEX_COUNT + (i - level)].vertex = vec_f;
			//Calculate the average height and set it to G.
			glm::vec3 vec_g = this->vertices[(j - level)*VERTEX_COUNT + (i - level / 2)];
			float height_g = (float)(height_a + height_b + height_e + this->vertices[(j - 3 * level / 2)*VERTEX_COUNT + (i - level / 2)].y) / 3;
			height_g += fmod(((float)(rand()) / 1000), MAX_DISPLACEMENT)*range;
			vec_g.y = height_g;
			this->vertices[(j - level)*VERTEX_COUNT + (i - level / 2)] = vec_g;
			this->containers[(j - level)*VERTEX_COUNT + (i - level / 2)].vertex = vec_g;
		}
	}
	//Begin Recursion.
	diamond_square(x1, x2, y1, y2, level / 2, range / 2);
}

void Terrain::updateHeights() {
	time_t currTime = time(nullptr);
	HeightGenerator hg = HeightGenerator(currTime * 18403481 % 421321);
	cout << "generator seed = " << hg.getSeed() << endl;
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].y = hg.generateHeight(vertices[i].x, vertices[i].z);
	}
	//updateNormals();
}

/* Updates the normals for the entire terrain. */
void Terrain::updateNormals()
{
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = 0; j < VERTEX_COUNT; j++)
		{
			//cout << VERTEX_COUNT << "\t" << i << "\t" << j << endl;
			//Get the proper heights.
			float heightL = getHeightFromVertex(j - 1, i);
			float heightR = getHeightFromVertex(j + 1, i);
			float heightD = getHeightFromVertex(j, i + 1);
			float heightU = getHeightFromVertex(j, i - 1);
			//Check if we're at the left edge.
			if (j == 0 && (terrain_left != nullptr)) {
				heightL = terrain_left->getHeightFromVertex((VERTEX_COUNT - 1), i);
			}
			//Check if we're at the right edge.
			if (j == (VERTEX_COUNT - 1) && (terrain_right != nullptr)) {
				heightR = terrain_right->getHeightFromVertex(0, i);
			}
			//Check if we're at the top edge.
			if (i == 0  && (terrain_top != nullptr)) {
				heightU = terrain_top->getHeightFromVertex(j, VERTEX_COUNT-1);
			}
			//Check if we're at the bottom edge.
			if (i == (VERTEX_COUNT - 1) && (terrain_bottom != nullptr)) {
				heightD = terrain_bottom->getHeightFromVertex(j, 0);
			}
			//Update the normal.
			glm::vec3 normal = glm::normalize(glm::vec3(heightL - heightR, 2.0f, heightU - heightD));
			this->normals[(i*VERTEX_COUNT) + j] = normal;
			this->containers[(i*VERTEX_COUNT) + j].normal = normal;
		}
	}
}

/* Updates and finds the max and min height of the terrain. */
void Terrain::updateMaxMinHeight()
{
	float max = -INFINITY, min = INFINITY;
	for (int i = 0; i < vertices.size(); i++)
	{
		float cur_height = vertices[i].y;
		if (cur_height > max)
		{
			max = cur_height;
		}
		if (cur_height < min)
		{
			min = cur_height;
		}
	}
	//Set the max and min heights found.
	this->max_height = max;
	this->min_height = min;
}

/** Load a ppm file from disk.
@input filename The location of the PPM file.  If the file is not found, an error message
will be printed and this function will return 0
@input width This will be modified to contain the width of the loaded image, or 0 if file not found
@input height This will be modified to contain the height of the loaded image, or 0 if file not found
@return Returns the RGB pixel data as interleaved unsigned chars (R0 G0 B0 R1 G1 B1 R2 G2 B2 .... etc) or 0 if an error ocured
**/
unsigned char * Terrain::loadPPM(const char* filename, int& width, int& height)
{
	const int BUFSIZE = 128;
	FILE* fp;
	unsigned int read;
	unsigned char* rawData;
	char buf[3][BUFSIZE];
	char* retval_fgets;
	size_t retval_sscanf;
	//Read in the ppm file.
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
		width = 0;
		height = 0;
		return NULL;
	}
	//Read magic number:
	retval_fgets = fgets(buf[0], BUFSIZE, fp);
	//Read width and height:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
	width = atoi(buf[1]);
	height = atoi(buf[2]);
	//Read maxval:
	do
	{
		retval_fgets = fgets(buf[0], BUFSIZE, fp);
	} while (buf[0][0] == '#');
	//Read image data:
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
	return rawData;//Return rawData or 0 if failed.
}

/* Load the Terrain through the read height map. */
GLuint Terrain::loadTerrain(const char* filename, int index)
{
	//Hold the textureID (This will be the textureID to return).
	GLuint textureID;
	//Define variables to hold height map's width, height, pixel information.
	int width, height;
	unsigned char * image;
	//Create ID for texture.
	glGenTextures(1, &textureID);
	//Set the active texture ID.
	glActiveTexture(GL_TEXTURE0);
	//Set this texture to be the one we are working with.
	glBindTexture(GL_TEXTURE_2D, textureID);
	//Generate the texture.
	image = loadPPM(filename, width, height);//Load the ppm file.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//Make sure no bytes are padded:
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Use bilinear interpolation:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//Use clamp to edge:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//X
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//Y
	//Unbind the texture cube map.
	glBindTexture(GL_TEXTURE_2D, 0);
	//Return the textureID, we need to keep track of this texture variable.
	return textureID;
}

void Terrain::setupTerrain(const char* terrain) {
	//Create buffers/arrays.
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	//Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO); //Bind vertex array object.

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Bind Container buffer.
	glBufferData(GL_ARRAY_BUFFER, this->containers.size() * sizeof(Container), &this->containers[0], GL_STATIC_DRAW); //Set vertex buffer to the Container.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Bind indices buffer.
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(int), &this->indices[0], GL_STATIC_DRAW);

	//Vertex Positions.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,//This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
		3, //This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component).
		GL_FLOAT, //What type these components are.
		GL_FALSE, //GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't.
		sizeof(Container), //Offset between consecutive vertex attributes. Since each of our vertices have 3 floats, they should have the size of 3 floats in between.
		(GLvoid*)0); //Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.

	//Vertex Normals.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, normal));

	//Vertex Texture Coords.
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Container), (GLvoid*)offsetof(Container, texCoord));

	//Set up Terrain textures.
	this->terrainTexture = loadTerrain(terrain, 0);

	//Unbind.
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind.
	glBindVertexArray(0); //Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO.
}

/* Toggle the draw mode to draw the mesh as lines (wireframe) or as triangle faces. */
void Terrain::toggleDrawMode()
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

/* Draw the terrain. */
void Terrain::draw(GLuint shaderProgram)
{
	//Calculate combination of the model (toWorld), view (camera inverse), and perspective matrices. Send to shader.
	glm::mat4 MVP = Window::P * Window::V * this->toWorld;
	glm::mat4 model = this->toWorld;//We don't really need this, but we'll pass it through just in case.
	glm::mat4 view = glm::mat4(glm::mat3(Window::V));//Remove translation from the view matrix.
	glm::mat4 projection = Window::P;
	//Set MVP(Total calculated, easier to multiply in the shader) for the shader.
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	//Set individual components for shader calculations (Model, View, Projection).
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
	//Update heights.
	glUniform1f(glGetUniformLocation(shaderProgram, "max_height"), this->max_height);
	glUniform1f(glGetUniformLocation(shaderProgram, "min_height"), this->min_height);
	//Update viewPos.
	glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), Window::camera_pos.x, Window::camera_pos.y, Window::camera_pos.z);
	//Update toon_shade.
	glUniform1i(glGetUniformLocation(shaderProgram, "toon_shade"), Window::toonShadingOn);
	//Set draw_mode to view wireframe version or filled version.
	if (draw_mode == DRAW_SHADED)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//Draw the terrain.
	glBindVertexArray(VAO);//Bind the vertex.
	
	glActiveTexture(GL_TEXTURE);//Enable the texture.
	glBindTexture(GL_TEXTURE_2D, this->terrainTexture);
	glUniform1i(glGetUniformLocation(shaderProgram, "TerrainTexture"), 0);

	glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);//Unbind vertex.

	//Set it back to fill.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_FOG);
}

/* Update the shader with new updated vertices. */
void Terrain::update()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, this->containers.size() * sizeof(Container), &this->containers[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/* Stitches any attached terrains. */
void Terrain::stitch_all()
{
	stitch_left();
	stitch_right();
	stitch_top();
	stitch_bottom();
	//Update normals
	updateNormals();
	update();
}

/* Stitches the terrain to the left of it. */
void Terrain::stitch_left()
{
	//Base case to check if there's a defined left.
	if (!terrain_left)
		return;
	//Perform stitching.
	glm::vec3 cur_left = this->vertices[0];
	glm::vec3 next_right = this->terrain_left->vertices[(VERTEX_COUNT - 1)];
	float midpoint = next_right.y;

	this->vertices[0].y = midpoint;
	this->containers[0].vertex.y = midpoint;
	this->update();

	this->terrain_left->vertices[(VERTEX_COUNT - 1)].y = midpoint;
	this->terrain_left->containers[(VERTEX_COUNT - 1)].vertex.y = midpoint;
	this->terrain_left->update();

	for (int i = 1; i < VERTEX_COUNT; i++)
	{
		glm::vec3 cur_left = this->vertices[(VERTEX_COUNT*i)];
		glm::vec3 next_right = this->terrain_left->vertices[(VERTEX_COUNT *i) + (VERTEX_COUNT - 1)];
		float midpoint = (cur_left.y + next_right.y) / 2.0f;

		this->vertices[(VERTEX_COUNT*i)].y = midpoint;
		this->containers[(VERTEX_COUNT*i)].vertex.y = midpoint;
		this->update();

		this->terrain_left->vertices[(VERTEX_COUNT *i) + (VERTEX_COUNT - 1)].y = midpoint;
		this->terrain_left->containers[(VERTEX_COUNT *i) + (VERTEX_COUNT - 1)].vertex.y = midpoint;
		this->terrain_left->update();
	}
}

/* Stitches the terrain to the right of it. */
void Terrain::stitch_right()
{
	//Base case to check if there's a defined right.
	if (!terrain_right)
		return;
	//Perform stitching.
	for (int i = 0; i < VERTEX_COUNT-1; i++)
	{
		glm::vec3 cur_right = this->vertices[(VERTEX_COUNT*i) + (VERTEX_COUNT - 1)];
		glm::vec3 next_left = this->terrain_right->vertices[(VERTEX_COUNT*i)];
		float midpoint = (cur_right.y + next_left.y) / 2.0f;

		this->vertices[(VERTEX_COUNT*i) + (VERTEX_COUNT - 1)].y = midpoint;
		this->containers[(VERTEX_COUNT*i) + (VERTEX_COUNT - 1)].vertex.y = midpoint;
		this->update();

		this->terrain_right->vertices[(VERTEX_COUNT*i)].y = midpoint;
		this->terrain_right->containers[(VERTEX_COUNT*i)].vertex.y = midpoint;
		this->terrain_right->update();
	}

	glm::vec3 cur_right = this->vertices[(VERTEX_COUNT*(VERTEX_COUNT-1)) + (VERTEX_COUNT - 1)];
	glm::vec3 next_left = this->terrain_right->vertices[(VERTEX_COUNT*(VERTEX_COUNT - 1))];
	float midpoint = next_left.y;

	this->vertices[(VERTEX_COUNT*(VERTEX_COUNT - 1)) + (VERTEX_COUNT - 1)].y = midpoint;
	this->containers[(VERTEX_COUNT*(VERTEX_COUNT - 1)) + (VERTEX_COUNT - 1)].vertex.y = midpoint;
	this->update();

	this->terrain_right->vertices[(VERTEX_COUNT*(VERTEX_COUNT - 1))].y = midpoint;
	this->terrain_right->containers[(VERTEX_COUNT*(VERTEX_COUNT - 1))].vertex.y = midpoint;
	this->terrain_right->update();

}

/* Stitches the terrain above it. AKA, negative z from this one. */
void Terrain::stitch_top()
{
	//Base case to check if there's a defined top.
	if (!terrain_top)
		return;
	//Perform stitching.
	for (int i = 0; i < VERTEX_COUNT-1; i++)
	{
		glm::vec3 cur_top = this->vertices[i];
		glm::vec3 next_bottom = this->terrain_top->vertices[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + i];
		float midpoint = (cur_top.y + next_bottom.y) / 2.0f;

		this->vertices[i].y = midpoint;
		this->containers[i].vertex.y = midpoint;
		this->update();

		this->terrain_top->vertices[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + i].y = midpoint;
		this->terrain_top->containers[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + i].vertex.y = midpoint;
		this->terrain_top->update();
	}

	glm::vec3 cur_top = this->vertices[VERTEX_COUNT - 1];
	glm::vec3 next_bottom = this->terrain_top->vertices[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + (VERTEX_COUNT - 1)];
	float midpoint = next_bottom.y;

	this->vertices[(VERTEX_COUNT - 1)].y = midpoint;
	this->containers[(VERTEX_COUNT - 1)].vertex.y = midpoint;
	this->update();

	this->terrain_top->vertices[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + (VERTEX_COUNT - 1)].y = midpoint;
	this->terrain_top->containers[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + (VERTEX_COUNT - 1)].vertex.y = midpoint;
	this->terrain_top->update();
}

/* Stitches the terrain above it. AKA, positive z from this one. */
void Terrain::stitch_bottom()
{
	//Base case to check if there's a defined bottom.
	if (!terrain_bottom)
		return;
	//Perform stitching.
	for (int i = 1; i < VERTEX_COUNT; i++)
	{
		glm::vec3 cur_bottom = this->vertices[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + i];
		glm::vec3 next_top = this->terrain_bottom->vertices[i];
		float midpoint = (cur_bottom.y + next_top.y) / 2.0f;

		this->vertices[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + i].y = midpoint;
		this->containers[(VERTEX_COUNT)*(VERTEX_COUNT - 1) + i].vertex.y = midpoint;
		this->update();

		this->terrain_bottom->vertices[i].y = midpoint;
		this->terrain_bottom->containers[i].vertex.y = midpoint;
		this->terrain_bottom->update();
	}
}

/* Returns the interpolated height for BaryCentric coordinates. */
float Terrain::BaryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos)
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);

	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;

	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

/* Gets the height of the terrain at a given position. */
float Terrain::getHeight(glm::vec3 position)
{
	//Get the coordinates in terms of the terrain.
	float terrain_x = position.x - this->x;
	float terrain_z = position.z - this->z;
	//Check for boundaries.
	if (terrain_x > SIZE || terrain_z > SIZE) {
		printf("Accessing a point out of bounds of this terrain. Please check to make sure we're accessing the proper terrain.\n");
		return 0;
	}
	//Get grid coordinate to determine which vertices to interpolate.
	float gridSize = (float)(SIZE) / (float)(VERTEX_COUNT-1);
	float gridX = floor(terrain_x / gridSize);
	float gridZ = floor(terrain_z / gridSize);
	//Get xCoord and zCoord within the square.
	float xCoord = fmod(terrain_x, gridSize) / gridSize;
	float zCoord = fmod(terrain_z, gridSize) / gridSize;
	//Compute the height between the triangles.
	float answer;
	if (xCoord <= (1 - zCoord))
	{
		answer = BaryCentric(glm::vec3(0.0f, this->vertices[gridZ*VERTEX_COUNT + gridX].y, 0.0f), glm::vec3(1.0f, this->vertices[gridZ*VERTEX_COUNT + gridX + 1].y, 0.0f), glm::vec3(0.0f, this->vertices[(gridZ + 1)*VERTEX_COUNT + gridX + 1].y, 1.0f), glm::vec2(xCoord, zCoord));
	}
	else
	{
		answer = BaryCentric(glm::vec3(1.0f, this->vertices[gridZ*VERTEX_COUNT + gridX + 1].y, 0.0f), glm::vec3(1.0f, this->vertices[(gridZ + 1)*VERTEX_COUNT + gridX + 1].y, 1.0f), glm::vec3(0.0f, this->vertices[(gridZ + 1)*VERTEX_COUNT + gridX].y, 1.0f), glm::vec2(xCoord, zCoord));
	}
	//Return the result.
	cout << "answer: " << answer << endl;
	return answer;
}
