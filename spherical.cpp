#include "spherical.hpp"

std::vector<glm::vec3> Spherical::GetPolarPolygon(std::vector<glm::vec3>& polygon)
{
	// Using the polar dual formula: w_i' = w_i \times w_{i+1}.
	// w_k' = w_k \times w_0, where k is the last index.
	
	std::vector<glm::vec3> polar;
	polar.reserve(polygon.size());

	for (int i = 0; i < polygon.size() - 1; ++i)
	{
		glm::vec3 dual = glm::cross(polygon[i], polygon[i + 1]);	
		dual /= glm::length(dual);
		polar.push_back(dual);
	}
	
	// Compute last element manually.
	glm::vec3 last = glm::cross(polygon[polygon.size() - 1], polygon[0]);
	last /= glm::length(last);
	polar.push_back(last);

	/*
	std::cout << "Found " << polar.size() << " vertices in the polar dual:" << std::endl;
	for (glm::vec3& v : polar)
		std::cout << glm::to_string(v) << std::endl;
	*/

	return polar;
}

std::vector<glm::vec3> Spherical::GetGaussMap(std::vector<glm::vec3>& star, uint count, float sphereRadius)
{
	// First get the Gauss map.
	// Duplicate the first element at the end, so that we will connect normals[last] -> normals[first].
	std::vector<glm::vec3> normals;
	normals.reserve(star.size() + 1);
	for (int i = 0; i < star.size(); ++i)
	{
		normals.push_back(star[i]);
	}
	normals.push_back(star[0]);

	// Compute geodesic arcs between normals[i] to normals[i+1].
	std::vector<glm::vec3> gaussMap;
	//gaussMap.reserve(star.size() * count);
	for (int i = 0; i < normals.size() - 1; ++i)
	{
		glm::vec3& current = normals[i];
		glm::vec3& next = normals[i + 1];

		//std::cout << "Positive arc length: " << glm::acos(glm::dot(current, next)) << std::endl;

		// Interpolation.
		gaussMap.push_back(sphereRadius * current);
		for (int j = 1; j < count; ++j)
		{
			// Linear interpolate between current and next.
			float t = (float)j / (float)count;
			glm::vec3 position = (1.0f - t) * current + t * next;	

			// Take the spherical projection.
			position *= sphereRadius / glm::length(position);
			//std::cout << glm::to_string(position) << std::endl;

			gaussMap.push_back(position);
		}
	}
	gaussMap.push_back(sphereRadius * normals[0]);

	return gaussMap;
}

std::vector<glm::vec3> Spherical::GetGaussMap(std::vector<Triangle*>& star, uint count, float sphereRadius)
{
	// First get the Gauss map.
	// Duplicate the first element at the end, so that we will connect normals[last] -> normals[first].
	std::vector<glm::vec3> normals;
	normals.reserve(star.size() + 1);
	for (int i = 0; i < star.size(); ++i)
	{
		normals.push_back((glm::vec3)star[i]->normal);
	}
	normals.push_back((glm::vec3)star[0]->normal);

	// Compute geodesic arcs between normals[i] to normals[i+1].
	std::vector<glm::vec3> gaussMap;
	//gaussMap.reserve(star.size() * count);
	for (int i = 0; i < normals.size() - 1; ++i)
	{
		glm::vec3& current = normals[i];
		glm::vec3& next = normals[i + 1];

		// Interpolation.
		gaussMap.push_back(sphereRadius * current);
		for (int j = 1; j < count; ++j)
		{
			// Linear interpolate between current and next.
			float t = (float)j / (float)count;
			glm::vec3 position = (1.0f - t) * current + t * next;	

			// Take the spherical projection.
			position *= sphereRadius / glm::length(position);
			//std::cout << glm::to_string(position) << std::endl;

			gaussMap.push_back(position);
		}
	}
	gaussMap.push_back(sphereRadius * normals[0]);

	return gaussMap;
}












Spherical::Spherical(){}
Spherical::~Spherical(){}
