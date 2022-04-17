#pragma once

#include <vector>
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "vertex.hpp"
#include "polyhedron.hpp"

class Polyhedron;

// contains data necessary for rendering:
// * model vertices.
// * triangle configuration .
// * vaoID.
// * vboID: for the vertex data.
// * model transform.
class MeshComponent
{
public:

	MeshComponent();

	// Blind copies: direct copy information into a MeshComponent.
	MeshComponent(std::vector<Vertex> vertices, std::vector<uint> triangles);
	MeshComponent(Polyhedron* p);

	// Triangle-based: duplicate vertices to allow for per-triangle color.
	MeshComponent(Polyhedron* p, std::vector<double>& triangleHorizon);

	// Assign colors based upon horizon measure.
	void AssignHorizonMeasureColors(std::vector<float>& triangleHorizon);

	// getters/setters:
	uint getVAO();
	uint getVBO();
	uint getCount();
	void setVAO(uint vaoID);
	void setVBO(uint vboID);

	std::vector<Vertex>& getVertices();
	std::vector<uint>& getTriangles();

	float InverseLerp(float start, float end, float v);
	double InverseLerp(double start, double end, double v);
	
	void CreateModel(std::vector<Vertex> vertices, std::vector<uint> triangles);

	glm::mat4 transform;

private:

	glm::vec3 InterpolateColor(double min, double mean, double max, double value);
	glm::vec3 InterpolateColor(float min, float mean, float max, float value);
	glm::vec3 InterpolateColorPercentage(double min, double mean, double max, double maxDistanceFromMean, double value);
	glm::vec3 InterpolateColorPercentage(float min, float mean, float max, float maxDistanceFromMean, float value);

	// raw model data:
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

	// OpenGL rendering data:
	uint vaoID;
	uint vboID; // Vertex data VBO.

};
