#pragma once

#include <vector>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
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
	MeshComponent(Polyhedron* p, glm::vec3 color);
	MeshComponent(Polyhedron* p);

	// Vertex-based coloring using the values.
	MeshComponent(Polyhedron* p, std::vector<double>& values, Curvature c);
	glm::vec4 InterpolateSignedColor(double minNegative, double maxPositive, double meanNegative, double meanPositive, double value);
	glm::vec4 InterpolateColor(double min, double max, double mean, double value);

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
	glm::vec3 Lerp(glm::vec3 start, glm::vec3 end, float t);
	
	glm::mat4 transform;

private:

	// raw model data:
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

	// OpenGL rendering data:
	uint vaoID;
	uint vboID; // Vertex data VBO.

};
