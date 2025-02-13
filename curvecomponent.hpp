#pragma once

#include <vector>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "linevertex.hpp"

// contains data necessary for rendering curves:
// * model vertices.
// * vaoID.
// * vboID: for the vertex data.
// * model transform.
class CurveComponent
{
public:

	CurveComponent();

	// Blind copies: direct copy information into a CurveComponent.
	CurveComponent(std::vector<LineVertex> vertices);

	// getters/setters:
	uint getVAO();
	uint getVBO();
	uint getCount();
	void setVAO(uint vaoID);
	void setVBO(uint vboID);

	std::vector<LineVertex>& getVertices();

	float InverseLerp(float start, float end, float v);
	double InverseLerp(double start, double end, double v);
	glm::vec3 Lerp(glm::vec3 start, glm::vec3 end, float t);
	
	glm::mat4 transform;

private:

	// raw model data:
	std::vector<LineVertex> vertices;

	// OpenGL rendering data:
	uint vaoID;
	uint vboID; // Vertex data VBO.

};
