#pragma once
#ifndef HEIGHTGENERATOR_H
#define HEIGHTGENERATOR_H

#include "Window.h"
#include "Definitions.h"

class HeightGenerator
{
private:
	int seed;
	int xOffset;
	int zOffset;
public:
	HeightGenerator(int seed);
	HeightGenerator(int gridX, int gridZ, int vertexCount, int seed);
	float generateHeight(float x, float z);
	float getInterpolatedNoise(float x, float z);
	float interpolate(float a, float b, float blend);
	float getSmoothNoise(int x, int z);
	float getNoise(int x, int z);
	int getSeed();
};

#endif
