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

	float Noise(float x, float y, float z);

private:

	float Fade(float t);
	float Lerp(float t, float a, float b);
	float Gradient(int hash, float x, float y, float z);

	// Permutation array.
	std::vector<int> p;

};
