#pragma once

#include <cmath>
#include <vector>
#include <iostream>

// A Perlin Noise implementation.
class PerlinNoise
{

public:

	PerlinNoise();
	~PerlinNoise();

	// One pass of the basic Perlin Noise algorithm.
	float Noise(float x, float y, float z);

	// More complex noise using octaves and persistance.
	float LayeredNoise(float x, float y, float z, int octaves, float persistence, float lacunarity);

private:

	float Fade(float t);
	float Lerp(float t, float a, float b);
	float Gradient(int hash, float x, float y, float z);

	// Permutation array.
	std::vector<int> p;

};
