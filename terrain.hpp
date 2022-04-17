#pragma once

#include <cstdlib>
#include "meshcomponent.hpp"
#include "perlinnoise.hpp"

// Biomes for the terrain.
enum class Biome
{
	Grassland = 0,
	Mountain = 1
};

// Creates terrain with biomes.
class Terrain
{

public:

	Terrain(int width, int height);
	~Terrain();



private:

	std::vector<std::vector<Biome>> map;

};

