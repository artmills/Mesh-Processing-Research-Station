#pragma once

#include <map>
#include "polyhedron.hpp"

/** Container for all information needed to represent a given polyhedron from a given viewing direction.
 * Information:
 * 1) Viewing position.
 * 2) Viewing direction.
 * 3) Polyhedron*.
 * Once this is constructed, a silhouette may be formed by analyzing front- vs. back-facing triangles. 
 */
struct View
{

public:

	View();
	View(glm::dvec3 position, glm::dvec3 target, Polyhedron* poly);
	~View();

	glm::dvec3 getViewDirection();

	glm::dvec3 viewPosition;
	glm::dvec3 viewTarget;
	Polyhedron* p;
};
