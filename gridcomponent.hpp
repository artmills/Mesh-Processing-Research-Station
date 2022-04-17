#pragma once
#include <iostream>
#include <vector>

#include "block.hpp"

/** Representing a 2D terrain. */
class GridComponent
{

public:

	/** Create a blank grid with the specified number of rows and columns. 
	 *
	 * This grid takes values in the block struct. */
	GridComponent(int width, int height);
	~GridComponent();

	
	/** Get the number of rows of the grid. */
	int getWidth();


	/** Get the number of columns of the grid. */
	int getHeight();


	/** Get the element at this location. */
	Biome getElement(int row, int column);


	/** Set the element at this location to the given value. */
	void setElement(int row, int column, Biome biome);


	/** Utility.
	 * Print out the grid as a matrix. */
	void Print();

private:

	/** The actual grid data as a 1D list of blocks. */
	std::vector<Biome> map;

	/** Width of the grid. */
	int width;
	
	/** Height of the grid. */
	int height;
};
