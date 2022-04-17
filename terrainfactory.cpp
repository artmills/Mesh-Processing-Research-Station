#include "terrainfactory.hpp"

float TerrainFactory::waterHeightNorm = 0.3f;
float TerrainFactory::grassHeightNorm = 0.5f;
float TerrainFactory::mountainHeightNorm = 1.0f;

TerrainFactory::TerrainFactory()
{
	std::srand(time(0));
}
TerrainFactory::~TerrainFactory(){}

Point::Point(int x, int y)
{
	this->x = x;
	this->y = y;
}

// Identify water regions and the boundary points for them.
std::vector<Point> TerrainFactory::GetWaterBoundary(GridComponent& map)
{
	// Go through the biome map and search for unactivated water tiles.
	// Once a water tile is found, activate it and its neighbors.
	// While activating, keep track of the min/max x/z-values.
	GridComponent activated(map.getWidth(), map.getHeight());
	std::vector<Point> corners;

	for (int i = 0; i < map.getWidth(); ++i)
	{
		for (int j = 0; j < map.getHeight(); ++j)
		{
			// If this is activated OR is not a water tile, skip.
			if (map.getElement(i, j) != Biome::Ocean || activated.getElement(i, j) == (Biome)1)
				continue;

			// Unactivated ocean tile: look at all neighbors using blob method.
			Point min(10000, 10000);
			Point max(-10000, -10000);
			std::vector<Point> boundary = { Point(i, j) };
			activated.setElement(i, j, (Biome)1);
			while (boundary.size() > 0)
			{
				std::vector<Point> newBoundary;
				for (Point p : boundary)
				{
					// Check if adjacent tiles have been activated.
					for (int s = -1; s <= 1; ++s)
					{
						for (int t = -1; t <= 1; ++t)
						{
							// Skip the current tile.
							if (s == 0 && t == 0)
								continue;

							int x = p.x + s;
							int y = p.y + t;
							
							// Skip tiles that are out of bounds.
							if (x < 0 || x >= map.getWidth() || y < 0 || y >= map.getHeight())
								continue;

							// Skip non-water tiles and activated tiles.
							if (map.getElement(x, y) != Biome::Ocean || activated.getElement(x, y) == (Biome)1)
								continue;

							// Newly activated tiles are added to the boundary and checked against the min/max.
							if (activated.getElement(x, y) != (Biome)1)
							{
								activated.setElement(x, y, (Biome)1);
								newBoundary.push_back(Point(x, y));
								if (x < min.x)
									min.x = x;
								if (x > max.x)
									max.x = x;
								if (y < min.y)
									min.y = y;
								if (y > max.y)
									max.y = y;
							}
						}
					}
				}
				boundary = newBoundary;
			}
			corners.push_back(min);
			corners.push_back(max);
			std::cout << min.x << " " << min.y << " " << max.x << " " << max.y << std::endl;
		}
	}
	return corners;
}

std::vector<Vertex> TerrainFactory::GetTerrainVertices(
			int xNumTiles, int zNumTiles, float xTileSize, float zTileSize,
			int octaves, float persistence, float lacunarity)
{
	float maxMountainHeight = 240.0f;
	// Get height map and biome map:
	fMap heightMap = GetHeightMap(xNumTiles, zNumTiles, xTileSize, zTileSize, octaves, persistence, lacunarity);
	GridComponent biomeMap = GetBiomeMap(heightMap, maxMountainHeight);

	// Colors:
	glm::vec4 waterColor(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 groundColor1(0.3f, 0.44f, 0.0f, 1.0f);
	glm::vec4 groundColor2(0.38f, 0.48f, 0.0f, 1.0f);
	glm::vec4 stoneColor(0.6f, 0.6f, 0.6f, 1.0f);
	glm::vec4 mountainColor(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 snowColor(1, 1, 1, 1);
	
	// Biome heights:
	float waterHeight = maxMountainHeight * HeightCurve(waterHeightNorm);
	float grassHeight = maxMountainHeight * HeightCurve(grassHeightNorm);
	float mountainHeight = maxMountainHeight * HeightCurve(mountainHeightNorm);
	float stoneHeight = maxMountainHeight * HeightCurve(0.75f);
	float snowHeight = maxMountainHeight;

	std::vector<Vertex> vertices;
	float width = xNumTiles * xTileSize;
	float height = zNumTiles * zTileSize;

	// Water tiles.
	for (int j = 0; j <= zNumTiles; ++j)
	{
		for (int i = 0; i <= xNumTiles; ++i)
		{
			float xCoord = (float)i * xTileSize;
			float zCoord = (float)j * zTileSize;
			float yCoord = heightMap[i][j];

			// Determine color based upon biome:
			glm::vec4 color;
			Biome biome = biomeMap.getElement(i, j);

			if (yCoord == waterHeight)
				color = waterColor;
			else if (yCoord <= grassHeight)
			{
				if (i % 2 == 0 && j % 2 == 0)
					color = groundColor1;
				else
					color = groundColor2;
			}
			else
			{
				float c = InverseLerp(0, mountainHeight, yCoord);
				color = glm::vec4(1.6f * c, 1.6f * c, 1.6f * c, 1);
			}
			/*
			else if (yCoord <= stoneHeight)
				color = stoneColor;
			else if (yCoord <= mountainHeight)
				color = mountainColor;
			else if (yCoord <= snowHeight)
				color = snowColor;
			*/
			

			Vertex v(xCoord, yCoord, zCoord, color);
			v.setTexture((float)i / width, (float)j / height);
			vertices.push_back(v);
		}
	}


	return vertices;
}

float TerrainFactory::HeightCurve(float x)
{
	if (x <= waterHeightNorm)
		return x - waterHeightNorm;
	else 
		//return 80.0f * (exp(x - 0.4) - 1);
		return ((float)1 / ((1.0f - waterHeightNorm) * (1.0f - waterHeightNorm))) * (x - waterHeightNorm) * (x - waterHeightNorm);
}

// Set biomes based upon heights and also put in height multiplier.
GridComponent TerrainFactory::GetBiomeMap(fMap& heightMap, float maxMountainHeight)
{
	GridComponent biomeMap(heightMap.size(), heightMap[0].size());
	for (int i = 0; i < biomeMap.getWidth(); ++i)
	{
		for (int j = 0; j < biomeMap.getHeight(); ++j)
		{
			float height = HeightCurve(heightMap[i][j]);

			// Water:
			if (heightMap[i][j] <= waterHeightNorm)
			{
				biomeMap.setElement(i, j, Biome::Ocean);
			}
			
			// Grassland:
			else if (heightMap[i][j] <= grassHeightNorm)
				biomeMap.setElement(i, j, Biome::Grassland);

			// Mountain:
			else 
				biomeMap.setElement(i, j, Biome::Mountain);

			heightMap[i][j] = maxMountainHeight * height;
		}
	}
	return biomeMap;
}


fMap TerrainFactory::GetHeightMap(
			int xNumTiles, int zNumTiles, float xTileSize, float zTileSize,
			int octaves, float persistence, float lacunarity)
{
	PerlinNoise pn;
	float noiseAmplitude = 1.0f;

	float width = xNumTiles * xTileSize;
	float height = zNumTiles * zTileSize;

	// Initialize height map:
	fMap heightMap(xNumTiles + 1);
	for (int i = 0; i < heightMap.size(); ++i)
	{
		heightMap[i] = std::vector<float>(zNumTiles + 1);
	}

	// To normalize the noise values to be in [-1, 1], keep track of min/max:
	float min = 100000;
	float max = -100000;

	// Compute heights:
	for (int j = 0; j <= zNumTiles; ++j)
	{
		for (int i = 0; i <= xNumTiles; ++i)
		{
			float xCoord = (float)i * xTileSize;
			float zCoord = (float)j * zTileSize;
			float yCoord = noiseAmplitude * pn.LayeredNoise(xCoord / (float)width, zCoord / (float)height, 0, octaves, persistence, lacunarity);

			if (yCoord < min)
				min = yCoord;
			if (yCoord > max)
				max = yCoord;

			heightMap[i][j] = yCoord;
		}
	}

	// Normalize height map:
	for (int i = 0; i <= xNumTiles; ++i)
	{
		for (int j = 0; j <= zNumTiles; ++j)
		{
			heightMap[i][j] = InverseLerp(min, max, heightMap[i][j]);
		}
	}

	return heightMap;
}


/*
std::vector<std::vector<float>> TerrainFactory::SmoothHeightMap(std::vector<std::vector<float>>& heightMap, GridComponent& biomeMap, int radius, int xNumTiles, int zNumTiles, float xTileSize, float zTileSize, int octaves, float persistence, float lacunarity)
{
	PerlinNoise pn; 

	std::vector<std::vector<float>> smoothed = heightMap;
	float grasslandNoiseAmplitude = 12.0f;
	float grasslandNoiseFactor = 3.27f;
	float mountainNoiseAmplitude = 120.0f;
	float mountainNoiseFactor = 7.27f;

	// Take the average of the height values unless the point is completely surrounded by points of the same biome.
	for (int i = 0; i < biomeMap.getWidth(); ++i)
	{
		for (int j = 0; j < biomeMap.getHeight(); ++j)
		{
			int count = 0;
			float newHeight = 0;
			Biome currentBiome = biomeMap.getElement(i, j);

			// Compare the neighbors' biomes to the biome of this point.
			bool surroundedBySameBiome = true;
			for (int s = -radius; s <= radius; ++s)
			{
				for (int t = -radius; t <= radius; ++t)
				{
					// Bias in favor of mountains by pretending that this point would have been a mountain.
					if (s == 0 && t == 0)
					{
						float xCoord = (float)i * xTileSize;
						float zCoord = (float)j * zTileSize;
						float mountainHeight = mountainNoiseAmplitude * pn.LayeredNoise(mountainNoiseFactor * xCoord / (float)biomeMap.getWidth(), mountainNoiseFactor * zCoord / (float)biomeMap.getHeight(), 0, octaves, persistence, lacunarity);
						newHeight += 1.5f * mountainHeight;
						++count;
					}

					// Skip tiles out of bounds.
					Biome b = biomeMap.getElement(i + s, j + t);
					if (b == Biome::UNDEFINED)
						continue;
					if (i+s < 0 || j+t < 0 || i+s >= biomeMap.getWidth() || j+t >= biomeMap.getHeight())
						continue;

					// Check if NOT completely surrounded.
					if (b != currentBiome)
						surroundedBySameBiome = false;

					// Do computations for average:
					newHeight += heightMap[i + s][j + t];
					++count;
				}
			}

			// If surrounded by the same biome, no need to compute the average.
			if (!surroundedBySameBiome)
				smoothed[i][j] = newHeight / (float)count;	
		}
	}
	return smoothed;
}


std::vector<std::vector<float>> TerrainFactory::GetHeightMap(GridComponent& map, int xNumTiles, int zNumTiles, float xTileSize, float zTileSize, int octaves, float persistence, float lacunarity)
{
	PerlinNoise pn;

	float grasslandNoiseAmplitude = 12.0f;
	float grasslandNoiseFactor = 3.27f;
	float mountainNoiseAmplitude = 120.0f;
	float mountainNoiseFactor = 7.27f;

	float width = (float)map.getWidth();
	float height = (float)map.getHeight();
	
	std::vector<std::vector<float>> heightMap;
	for (int i = 0; i < map.getWidth(); ++i)
	{
		heightMap.push_back(std::vector<float>(map.getHeight()));
	}

	// Get noise values for mountains and hills according to the biome map.
	for (int i = 0; i < map.getWidth(); ++i)
	{
		for (int j = 0; j < map.getHeight(); ++j)
		{
			Biome biome = map.getElement(i, j);
			float xCoord = (float)i * xTileSize;
			float zCoord = (float)j * zTileSize;

			float grassLandHeight = grasslandNoiseAmplitude * pn.LayeredNoise(grasslandNoiseFactor * xCoord / width, grasslandNoiseFactor * zCoord / height, 0, octaves, persistence, lacunarity);
			float terrainHeight = 0;

			if (biome == Biome::Grassland)
				terrainHeight = grassLandHeight;
			else if (biome == Biome::Mountain)
			{
				float mountainHeight = mountainNoiseAmplitude * pn.LayeredNoise(mountainNoiseFactor * xCoord / width, mountainNoiseFactor * zCoord / height, 0, octaves, persistence, lacunarity);
				terrainHeight = std::max(mountainHeight, grassLandHeight);
			}
			heightMap[i][j] = terrainHeight;
		}
	}
	
	return heightMap;
}



void TerrainFactory::GenerateBiomes(GridComponent& map, int mountainBiomes)
{
	for (int i = 0; i < mountainBiomes; ++i)
	{
		std::vector<Point> seeds;
		int xRandom = rand() % map.getWidth();
		int yRandom = rand() % map.getHeight();
		seeds.push_back(Point(xRandom, yRandom));	
		GrowBiome(map, seeds, Biome::Mountain, 1);
	}
	SmoothBiomes(map, Biome::Mountain);
}
*/



// This results in a great way to find boundaries for biomes, BUT leaves many "holes" of other biomes within each biome region.
// These holes need to be cleaned up by using a cellular automata algorithm.
/*
void TerrainFactory::GrowBiome(GridComponent& map, std::vector<Point> seeds,
					 Biome type, int radius)
{
	// A hopefully more efficient method of doing the first GrowBiome() method.
	// Only store a list of the freshly-grown tiles.
	std::vector<Point> boundary = seeds;
	float percent = 100.0f;

	// Keep growing until either the percent goes down or no growth has happened.
	// We may count points multiple times but this is okay: if a point is adjacent to multiple grown tiles it will
	// have a higher chance of growing.
	while (boundary.size() > 0 && percent > 0)
	{
		// Go through the boundary and attempt to activate its neighboring tiles.
		std::vector<Point> newGrowth;
		for (Point p : boundary)
		{
			// The point p is new growth so analyze neighbors out to two blocks.
			for (int i = -radius; i <= radius; ++i)
			{
				for (int j = -radius; j <= radius; ++j)
				{
					if (i == 0 && j == 0)
						continue;

					if (map.getElement(p.x + i, p.y + j) != type)
					{
						if ((rand() % 101) < percent)
						{
							map.setElement(p.x + i, p.y + j, type);
							newGrowth.push_back(Point(p.x + i, p.y + j));
						}
					}
				}
			}

			int random;
			// Left:
			if (map.getElement(p.x - 1, p.y) != type)
			{
				random = rand() % 101;
				if (random < percent)
				{
					map.setElement(p.x - 1, p.y, type);
					newGrowth.push_back(Point(p.x - 1, p.y));
				}
			}
			// Right:
			if (map.getElement(p.x + 1, p.y) != type)
			{
				random = rand() % 101;
				if (random < percent)
				{
					map.setElement(p.x + 1, p.y, type);
					newGrowth.push_back(Point(p.x + 1, p.y));
				}
			}
			// Up:
			if (map.getElement(p.x, p.y + 1) != type)
			{
				random = rand() % 101;
				if (random < percent)
				{
					map.setElement(p.x, p.y + 1, type);
					newGrowth.push_back(Point(p.x, p.y + 1));
				}
			}
			// Down:
			if (map.getElement(p.x, p.y - 1) != type)
			{
				random = rand() % 101;
				if (random < percent)
				{
					map.setElement(p.x, p.y - 1, type);
					newGrowth.push_back(Point(p.x, p.y - 1));
				}
			}
		}
		// Reset values for next iteration of while-loop:
		boundary = newGrowth;
		percent -= 1.0f;
		std::cout << percent << std::endl;
	}

}


void TerrainFactory::SmoothBiomes(GridComponent& map, Biome type)
{
	// Go through the map and change tiles to reflect their neighbors.
	GridComponent smoothed = map;
	for (int i = 0; i < map.getWidth(); ++i)
	{
		for (int j = 0; j < map.getHeight(); ++j)
		{
			int count = 0;
			Biome majority = GetMajorityBiome(map, i, j, 3, type, count);
			if ((majority == type || count > 2))
				smoothed.setElement(i, j, type);
		}
	}
	map = smoothed;
}

Biome TerrainFactory::GetMajorityBiome(GridComponent& grid, int x, int y, int neighborScanDistance, Biome type, int& typeCount)
{
	std::vector<int> biomeCounts((int)Biome::NUMBER_OF_BIOMES);	
	int count = 0;
	for (int i = x - neighborScanDistance; i <= x + neighborScanDistance; ++i)
	{
		// ignore tiles outside the bounds of the map.
		if (i < 0 || i >= grid.getWidth())
		{
			continue;
		}

		for (int j = y - neighborScanDistance; j <= y + neighborScanDistance; ++j)
		{
			// ignore tiles outside the bounds of the map.	
			if (j < 0 || j >= grid.getHeight())
			{
				continue;
			}

			// ignore itself.
			if (j == y && i == x)
			{
				continue;
			}

			// now check the type of the tile.
			if ((int)grid.getElement(i, j) > -1)
			{
				++biomeCounts[(int)grid.getElement(i, j)];
			}
		}
	}

	// Get the maximum count.
	Biome majority = (Biome)0;
	int max = biomeCounts[0];
	for (int i = 1; i < biomeCounts.size(); ++i)
	{
		if (biomeCounts[i] > max)
		{
			max = biomeCounts[i];
			majority = (Biome)i;
		}
	}
	typeCount = biomeCounts[(int)type];
	return majority;
}
*/


GridComponent TerrainFactory::CA(int x, int y, int percent, int affinity, int smooth)
{
	// create the grid and perform cellular automation.
	GridComponent grid(x, y);

	RandomFill(grid, percent);
	for (int i = 0; i < smooth; ++i)
	{
		grid = Smooth(grid, affinity, 1);
	}

	//grid.Print();

	return grid;
}

void TerrainFactory::RandomFill(GridComponent& grid, int percent)
{
	// seed the random generator.
	// This should really be done elsewhere.
	srand(time(NULL));

	// flip the elements in the grid.
	for (int x = 0; x < grid.getWidth(); ++x)
	{
		for (int y = 0; y < grid.getHeight(); ++y)
		{
			int random = rand() % 101;
			if (random < percent)
			{
				grid.setElement(x, y, Biome::Mountain);
			}
		}
	}
}

int TerrainFactory::GetNeighbors(GridComponent& grid, int x, int y, int neighborScanDistance)
{
	// since tiles are still considered to be path-connected
	// if they are diagonal, we will still consider diagonal
	// tiles as neighbors.
	Biome type = grid.getElement(x, y);
	int count = 0;
	for (int i = x - neighborScanDistance; i <= x + neighborScanDistance; ++i)
	{
		// ignore tiles outside the bounds of the map.
		if (i < 0 || i >= grid.getWidth())
		{
			continue;
		}

		for (int j = y - neighborScanDistance; j <= y + neighborScanDistance; ++j)
		{
			// ignore tiles outside the bounds of the map.	
			if (j < 0 || j >= grid.getHeight())
			{
				continue;
			}

			// ignore itself.
			if (j == y && i == x)
			{
				continue;
			}

			// now check if (i, j) is an activated square.
			if (grid.getElement(i, j) == type)
			{
				++count;
			}
		}
	}
	return count;
}


GridComponent TerrainFactory::Smooth(GridComponent& grid, int affinity, int neighborScanDistance)
{
	// get a blank grid. this grid will be filled in by
	// scanning the given grid. 
	GridComponent smoothGrid(grid.getWidth(), grid.getHeight());

	for (int x = 0; x < grid.getWidth(); ++x)
	{
		for (int y = 0; y < grid.getHeight(); ++y)
		{
			// check if we should convert the grid to its
			// neighbors' orientation.
			if (GetNeighbors(grid, x, y, neighborScanDistance) >= affinity)
			{
				smoothGrid.setElement(x, y, Biome::Mountain);	
			}
			else
			{
				smoothGrid.setElement(x, y, Biome::Grassland);
			}
		}
	}
	return smoothGrid;
}

float TerrainFactory::Lerp(float p, float q, float t)
{
	return (1.0f - t) * p + t * q;
}
float TerrainFactory::InverseLerp(float a, float b, float c)
{
	return (c - a) / (b - a);
}
