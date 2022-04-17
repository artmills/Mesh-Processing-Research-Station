#include "gridcomponent.hpp"


/********* constructors/destructors *********/
GridComponent::GridComponent(int width, int height)
{
	this->width = width;
	this->height = height;

	for (int i = 0; i < width * height; ++i)
	{
		map.push_back(Biome::Grassland);
	}
}
GridComponent::~GridComponent(){}





/********* getters/setters for rows/columns/elements *********/

int GridComponent::getWidth()
{
	return this->width;
}
int GridComponent::getHeight()
{
	return this->height;
}

Biome GridComponent::getElement(int x, int y)
{
	//if (x < 0 || y < 0 || x >= width || y >= height)
	//	return Biome::UNDEFINED;
	int index = width * y + x;
	if (index < 0 || index >= map.size())
		return Biome::UNDEFINED;
	else
		return this->map[width * y + x];
}
void GridComponent::setElement(int x, int y, Biome biome)
{
	int index = width * y + x;
	if (index >= 0 && index < map.size())
		this->map[width * y + x] = biome;
}



/********* Utility *********/

void GridComponent::Print()
{
	if (this->map.size() == 0)
	{
		std::cout << "[ ]" << std::endl;
	}
	else 
	{
		for (int y = height - 1; y >= 0; --y)
		{
			for (int x = 0; x < width; ++x)
			{
				std::cout << (int)getElement(x, y) << " ";
			}
			std::cout << std::endl;
		}
		/*
		for (int x = 0; x < height; ++x)
		{
			for (int y = 0; y < width; ++y)
			{
				std::cout << (int)this->map[width * x + y] << " ";
			}
			std::cout << std::endl;
		}
		*/
	}
	std::cout << std::endl;
}
