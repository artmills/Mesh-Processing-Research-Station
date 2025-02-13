#pragma once

#include <iostream>
#include <vector>
#include "glm/glm.hpp"
#include "geometry.hpp"
#include "vertex.hpp"

// Render spherical areas such as the Gauss map of the star of a vertex.
class Spherical
{

public:

	// Get the boundary of the spherical polygon for the Gauss map of the star of a vertex.
	// Count is the number of vertices on each geodesic arc (including endpoints).
	std::vector<glm::vec3> static GetGaussMap(std::vector<Triangle*>& star, uint count, float sphereRadius);
	std::vector<glm::vec3> static GetGaussMap(std::vector<glm::vec3>& star, uint count, float sphereRadius);

	// Given the vertices of a spherical polygon, compute the polar polygon.
	// The ith output vector is the dual to the ith input vector.
	std::vector<glm::vec3> static GetPolarPolygon(std::vector<glm::vec3>& polygon);

private:



	Spherical();
	~Spherical();

};
