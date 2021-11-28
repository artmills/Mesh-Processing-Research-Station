#include "meshfactory.hpp"

std::vector<MeshComponent> MeshFactory::GetSphere(float length, uint numPointsPerSide)
{
	// Assign faces as if looking at the xy-plane.
	std::vector<MeshComponent> cube;
	MeshComponent back = GetNormalizedSquare(length, numPointsPerSide, glm::vec3(0, 0, -1));
	MeshComponent left = GetNormalizedSquare(length, numPointsPerSide, glm::vec3(-1, 0, 0));
	MeshComponent front = GetNormalizedSquare(length, numPointsPerSide, glm::vec3(0, 0, 1));
	MeshComponent right = GetNormalizedSquare(length, numPointsPerSide, glm::vec3(1, 0, 0));
	MeshComponent top = GetNormalizedSquare(length, numPointsPerSide, glm::vec3(0, 1, 0));
	MeshComponent bottom = GetNormalizedSquare(length, numPointsPerSide, glm::vec3(0, -1, 0));

	cube.push_back(back);
	cube.push_back(left);
	cube.push_back(front);
	cube.push_back(right);
	cube.push_back(top);
	cube.push_back(bottom);

	return cube;
}



MeshComponent MeshFactory::GetNormalizedSquare(float length, uint numPointsPerSide, glm::vec3 normal)
{
	glm::vec3 axisA = glm::vec3(normal.y, normal.z, normal.x);
	glm::vec3 axisB = glm::cross(normal, axisA);

	std::vector<Vertex> vertices(numPointsPerSide * numPointsPerSide);
	std::vector<uint> triangles((numPointsPerSide - 1) * (numPointsPerSide - 1) * 6);
	int triIndex = 0;

	for (int y = 0; y < numPointsPerSide; ++y)
	{
		for (int x = 0; x < numPointsPerSide; ++x)
		{
			// Align face of the square to be on a cube according to normal.
			int vertexIndex = x + y * numPointsPerSide;
			glm::vec2 t(x / (numPointsPerSide - 1.0f), y / (numPointsPerSide - 1.0f));
			glm::vec3 position = normal + (2.0f * t.x - 1.0f) * axisA + (2.0f * t.y - 1.0f) * axisB;
			//position = glm::normalize(position);
			float x2 = position.x * position.x;
			float y2 = position.y * position.y;
			float z2 = position.z * position.z;
			float xx = position.x * sqrt(1.0f - 0.5f * (y2 + z2) + (y2 * z2) / 3.0f);
			float yy = position.y * sqrt(1.0f - 0.5f * (z2 + x2) + (z2 * x2) / 3.0f);
			float zz = position.z * sqrt(1.0f - 0.5f * (x2 + y2) + (x2 * y2) / 3.0f);

			// Set vertex information.
			Vertex v;

			/*
			v.setPosition(position.x, position.y, position.z);
			v.setNormal(position.x, position.y, position.z);
			*/

			v.setPosition(xx, yy, zz);
			v.setNormal(xx, yy, zz);

			v.setColor(0.0f, 1.0f, 0.0f, 1.0f);
			v.setTexture(0.0f, 0.0f);
			vertices[vertexIndex] = v;

			// Assemble triangles.
			if (x != numPointsPerSide - 1 && y != numPointsPerSide - 1)
			{
				triangles[triIndex + 0] = vertexIndex;	
				triangles[triIndex + 1] = vertexIndex + numPointsPerSide + 1;	
				triangles[triIndex + 2] = vertexIndex + numPointsPerSide;	
				triangles[triIndex + 3] = vertexIndex;	
				triangles[triIndex + 4] = vertexIndex + 1;	
				triangles[triIndex + 5] = vertexIndex + numPointsPerSide + 1;	
				triIndex += 6;
			}
		}
	}
	return MeshComponent(vertices, triangles);
}










MeshFactory::MeshFactory() {}
MeshFactory::~MeshFactory() {}
