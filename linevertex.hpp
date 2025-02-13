#pragma once

#include <vector>
#include <iostream>
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// Vertex for a point on a line.
class LineVertex
{

public:

	LineVertex(glm::vec3 position, glm::vec3 color);
	LineVertex(float x, float y, float z,
			   float r, float g, float b);
	~LineVertex();

	glm::vec3 GetPosition();
	glm::vec4 GetColor();

	void Print();

private:

	float x, y, z;
	float r, g, b, a;

};
