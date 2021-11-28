#pragma once

#include <vector>
#include <cmath>
#include "glm/glm.hpp"

#include "vertex.hpp"
#include "polyhedron.hpp"

class Polyhedron;

// contains data necessary for rendering:
// * model vertices.
// * triangle configuration .
// * vaoID.
// * model transform.
class MeshComponent
{
public:

	MeshComponent();
	MeshComponent(Polyhedron* p);

	// Triangle-based: duplicate vertices to allow for per-triangle color.
	MeshComponent(Polyhedron* p, std::vector<double>& triangleHorizon);
	MeshComponent(std::vector<Vertex> vertices, std::vector<uint> triangles);

	// getters/setters:
	uint getVAO();
	uint getCount();
	void setVAO(uint vaoID);

	std::vector<Vertex>& getVertices();
	std::vector<uint>& getTriangles();

	double InverseLerp(double start, double end, double v);
	
	void CreateModel(std::vector<Vertex> vertices, std::vector<uint> triangles);

	glm::mat4 transform;

private:

	// raw model data:
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

	// OpenGL rendering data:
	uint vaoID;

};
