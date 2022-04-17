#pragma once

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "meshcomponent.hpp"
#include "gridcomponent.hpp"
#include "perlinnoise.hpp"

typedef std::vector<std::vector<float>> fMap;

struct Point
{
	Point(int x, int y);
	int x, y;
};

/** Creates the mesh for the game's terrain.
 *
 * The process involves creating a grid of blocks using some terrain generation algorithm.
 * Available algorithms:
 *
 * 1) Cellular automation.
 *
 * Once the grid has been created, a mesh component will be created to suitably package the data for the GPU. */
class TerrainFactory
{

public:

	// Attempt #3: let the height map dictate the biomes.
	static fMap GetHeightMap(
			int xNumTiles, int zNumTiles, float xTileSize, float zTileSize,
			int octaves, float persistence, float lacunarity);

	static GridComponent GetBiomeMap(fMap& heightMap, float maxMountainHeight);

	static std::vector<Vertex> GetTerrainVertices(
			int xNumTiles, int zNumTiles, float xTileSize, float zTileSize,
			int octaves, float persistence, float lacunarity);

	static float HeightCurve(float x);

	static std::vector<Point> GetWaterBoundary(GridComponent& map);

	// Heights at which the biomes end.
	static float waterHeightNorm;
	static float grassHeightNorm;
	static float mountainHeightNorm;
	/*
	static float waterHeight = 0.2f;
	static float grassHeight = 0.4f;
	static float mountainHeight = 1.0f;
	*/



	// Given seeds of a particular biome, grow that biome around the seeds.
	//static void GrowBiome(GridComponent& map, std::vector<Point> seeds,
					 //Biome type, int radius);

	// Smooth over biomes using cellular automata.
	//static void SmoothBiomes(GridComponent& map, Biome type);

	// Randomly plant seeds to form biomes.
	//static void GenerateBiomes(GridComponent& map, int mountainBiomes);

	// Get the heightmap from the biomes.
	//static std::vector<std::vector<float>> GetHeightMap(GridComponent& map, int xNumTiles, int zNumTiles, float xTileSize, float zTileSize, int octaves, float persistence, float lacunarity);

	// Smooth the heightmap to have better transitions between biomes.
	//static std::vector<std::vector<float>> SmoothHeightMap(std::vector<std::vector<float>>& heightMap, GridComponent& biomeMap, int radius, int xNumTiles, int zNumTiles, float xTileSize, float zTileSize, int octaves, float persistence, float lacunarity);




	/** Create a grid from cellular automata.
	 * Parameters:
	 * 
	 * x,y : the size of the terrain grid.
	 *
	 * percent: initially, a grid is created that is randomly filled in with air and stone blocks. 
	 * This parameter chooses what percentage of the grid should be randomly filled in at this initial stage.
	 *
	 * affinity: next, each block is checked against its neighbors.
	 * If a majority of a block's neighbors are stone, the current block will become stone.
	 * Affinity controls how many of those neighbors must be stone for this to happen.
	 * Diagonals are counted and the block itself is not counted.
	 *
	 * smooth: the number of times the cellular automata algorithm should be run. */
	static GridComponent CA(int x, int y, int percent, int affinity, int smooth);

private:

	/** Randomly fill in a grid to be *percent* full. 
	 *
	 * Currently, this will only choose a block of the grid to be either air or stone. */
	static void RandomFill(GridComponent& grid, int percent);


	/** Count how many active (stone) neighbors are around the given point.
	 *
	 * neighborScanDistance should probably just be set to 1. 
	 * This counts all eight adjacent neighbors to a given block.
	 *
	 * Logic is there to avoid counting the block itself as a neighbor and to skip over the edges of the grid where there are no blocks. */
	static int GetNeighbors(GridComponent& grid, int x, int y, int neighborScanDistance);
	static Biome GetMajorityBiome(GridComponent& grid, int x, int y, int neighborScanDistance, Biome type, int& typeCount);

	
	/** Apply the cellular automata algorithm. */
	static GridComponent Smooth(GridComponent& grid, int affinity, int neighborScanDistance);

	/** Inverse lerp. */
	static float Lerp(float p, float q, float t);
	static float InverseLerp(float a, float b, float c);

	TerrainFactory();
	~TerrainFactory();

};
