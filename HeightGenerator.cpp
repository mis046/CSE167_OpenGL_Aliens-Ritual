#include "HeightGenerator.h"

#define PI 3.1415926535
#define AMPLITUDE 75.0
#define OCTAVES 3
#define ROUGHNESS 0.3

HeightGenerator::HeightGenerator(int seed)
{
	this->seed = rand() % 10000000 + seed;
	srand(seed);
	this->xOffset = 0;
	this->zOffset = 0;
}

HeightGenerator::HeightGenerator(int gridX, int gridZ, int vertexCount, int seed)
{
	this->seed = seed;
	srand(seed);
	this->xOffset = gridX * (vertexCount - 1);
	this->zOffset = gridZ * (vertexCount - 1);
}

float HeightGenerator::generateHeight(float x, float z)
{
	float total = 0.0f;
	float d = (float)pow(2, OCTAVES - 1);
	for (int i = 0; i < OCTAVES; i++) {
		float freq = (float)(pow(2, i) / d);
		float amp = (float)pow(ROUGHNESS, i) * AMPLITUDE;
		total += getInterpolatedNoise((x + xOffset) * freq, (z + zOffset) * freq) * amp;
	}
	return total;
	/*
	float res = getInterpolatedNoise(x/4.0f, z/4.0f) * AMPLITUDE;
	return res;
	*/
}

float HeightGenerator::getInterpolatedNoise(float x, float z)
{
	int intX = (int)x;
	int intZ = (int)z;
	float fracX = x - intX;
	float fracZ = z - intZ;
	//cout << x << "\t" << z << endl;
	//cout << fracX << "\t" << fracZ << endl;
	
	float v1 = getSmoothNoise(intX, intZ);
	float v2 = getSmoothNoise(intX + 4, intZ);
	float v3 = getSmoothNoise(intX, intZ + 4);
	float v4 = getSmoothNoise(intX + 4, intZ + 4);
	float i1 = interpolate(v1, v2, fracX);
	float i2 = interpolate(v3, v4, fracX);
	return interpolate(i1, i2, fracZ);
}

float HeightGenerator::interpolate(float a, float b, float blend)
{
	float theta = blend * PI;
	float f = (float)(1.0f - cos(theta)) * 0.5f;
	//cout << "f: " << f << endl;
	return a * (1.0f - f) + b * f;
}

float HeightGenerator::getSmoothNoise(int x, int z)
{
	float corners = (getNoise(x - 4, z - 4) + getNoise(x + 4, z - 4) + getNoise(x - 4, z + 4) + getNoise(x + 4, z + 4)) / 16.0f;
	float sides = (getNoise(x - 4, z) + getNoise(x + 4, z) + getNoise(x, z - 4) + getNoise(x, z + 4)) / 8.0f;
	float center = getNoise(x, z) / 4.0f;
	//cout << "x: " << x << ", z: " << z << ", height: " << corners + sides + center << endl;
	return corners + sides + center;
}

float HeightGenerator::getNoise(int x, int z)
{
	srand(x * 29484 + z * 472915 + seed);
	return (float)rand() / RAND_MAX * 2.0f - 1.0f;
}

int HeightGenerator::getSeed() {
	return this->seed;
}
