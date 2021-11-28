#pragma once

#include <map>
#include "polyhedron.hpp"

/** Loop subdivision.
 * Goal: given a mesh, output a new mesh that has been subdivided according to Loop subdivision. */
class Subdivision
{

public:

	// Loop subdivision algorithm.
	Polyhedron static LoopSubdivision(Polyhedron* p);
	static Polyhedron* LoopSubdivisionHeap(Polyhedron* p);

private:

	// Get the appropriate linear combination of adjacent vertices.
	glm::dvec3 static GetAdjacentLinearCombination(Edge* e);

	// Get the appropriate linear combination of opposite vertices.
	// NOTE: Only call this if the edge separates two triangles.
	glm::dvec3 static GetOppositeLinearCombination(Edge* e);

	// Get the appropriate linear combination for vertices on a boundary.
	// NOTE: Only call this if the edge is a boundary.
	glm::dvec3 static GetBoundaryLinearCombination(Edge* e);

	// The odd vertices are the ones that get added by Loop subdivision.
	// Creating an odd vertex involves the linear combinations described by the previous three functions.
	Vert static CreateOddVertex(Edge* e, int index);

	// A weight needed to recompute the positions of mesh vertices.
	// This choice of beta is not due to Loop himself, but to more recent papers.
	double static Beta(int valence);

	// Find all vertices that are connected to the given vertex by an edge.
	std::map<int, Vert*> static GetConnectedVertices(Vert* v);

	// Adjacent linear combination for a vertex that is not on a boundary.
	glm::dvec3 static GetAdjacentLinearCombination(Vert* v);

	// Boundary linear combination for a vertex that is not on a boundary.
	glm::dvec3 static GetBoundaryLinearCombination(Vert* v);

	// Recompute a vertex.
	Vert static CreateEvenVertex(Vert* v);


	Subdivision();
	~Subdivision();

};
