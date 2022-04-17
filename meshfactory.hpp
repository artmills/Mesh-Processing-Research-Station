#pragma once

#include "utilities.hpp"
#include "meshcomponent.hpp"
#include "glm/glm.hpp"

// Create meshes.
class MeshFactory
{

public:

	static MeshComponent GetPseudosphere(uint numPointsPerSide);

	static MeshComponent GetNormalizedSquare(float length, uint numPointsPerSide, glm::vec3 normal);
	static MeshComponent GetNormalizedSquareTriangles(float length, uint numPointsPerSide, glm::vec3 normal);

	static std::vector<MeshComponent> GetSphere(float length, uint numPointsPerSide);
	static MeshComponent GetSphereTriangles(float length, uint numPointsPerSide);

private:


	MeshFactory();
	~MeshFactory();

};
