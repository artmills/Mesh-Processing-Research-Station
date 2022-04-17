#include "terrain.hpp"


// Note that width, height refer to number of nodes.
// So a 2x3 square actually has 3x4 nodes.
Terrain::Terrain(int width, int height)
{
	// Seed random number generator.
	std::srand(time(0));
	for (int i = 0; i <= width; ++i)
	{
		this->map.push_back(std::vector<Biome>(height + 1));
	}
}
Terrain::~Terrain(){}


