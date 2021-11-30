#include "meshcomponent.hpp"

MeshComponent::MeshComponent()
{
	this->transform = glm::mat4(1);
}
MeshComponent::MeshComponent(std::vector<Vertex> vertices, std::vector<uint> triangles)
{
	this->vertices = vertices;
	this->triangles = triangles;
	this->transform = glm::mat4(1);
}

double MeshComponent::InverseLerp(double start, double end, double v)
{
	return (v - start) / (end  - start);
}

// Triangle-based mesh.
MeshComponent::MeshComponent(Polyhedron *p, std::vector<double>& triangleHorizon)
{
	// Go through the Polyhedron's vlist and tlist and convert them to the correct data.
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

	glm::vec3 color;

	// Compute mean and std:
	double sum = 0;
	double max = 0;
	double min = std::numeric_limits<double>::max();
	for (int i = 0; i < triangleHorizon.size(); ++i)
	{
		double x = triangleHorizon[i];
		sum += x;	
		if (x > max)
		{
			max = x;
		}
		if (x < min)
			min = x;
	}
	double mean = sum / (double)triangleHorizon.size();

	double stdSums = 0;
	for (int i = 0; i < triangleHorizon.size(); ++i)
	{
		stdSums += (triangleHorizon[i] - mean) * (triangleHorizon[i] - mean);
	}
	double standardDeviation = sqrt((1.0 / (double)triangleHorizon.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Length(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;

	int divergeCount = 0;
	double maxDistanceFromMean = max - mean;
	if (mean - min > maxDistanceFromMean)
		maxDistanceFromMean = mean - min;
	
	// Get the vertices of each triangle.
	for (int i = 0; i < p->tlist.size(); ++i)
	{
		Triangle& t = p->tlist[i];
		
		// Look up horizon measure and compare it to the average.
		double horizon = triangleHorizon[i];
		double distFromMean = abs(horizon - mean);
		double percent = InverseLerp(0, maxDistanceFromMean, distFromMean);
		percent = cbrt(percent);

		color = glm::vec3(1.0f, 1.0f - percent, 1.0f - percent);

		if (horizon == max)
		{
			std::cout << "Max horizon measure is " << horizon << " " << max << std::endl;
			color = glm::vec3(0, 0, 1);
		}
		

		// Within half a std: green.
		/*
		if ((distFromMean < mean + 0.5*standardDeviation) && (distFromMean > mean - 0.5*standardDeviation))
			color = glm::vec3(0.0f, 1.0f - (distFromMean / mean), 0.0f);
		// Within a std: green + blue.
		else if ((distFromMean < mean + standardDeviation) && (distFromMean > mean - standardDeviation))
			color = glm::vec3(0.0f, 7.0f, 3.0f);
		// Within 1.5 std: blue + green.
		else if ((distFromMean < mean + 1.5*standardDeviation) && (distFromMean > mean - 1.5*standardDeviation))
			color = glm::vec3(0.0f, 3.0f, 7.0f);
		// Within 2 std: blue.
		else if ((distFromMean < mean + 2*standardDeviation) && (distFromMean > mean - 2*standardDeviation))
			color = glm::vec3(0.0f, 0.0f, 1.0f);
		// Diverge.
		else 
		{
			color = glm::vec3(1.0f, 0.0f, 0.0f);
			++divergeCount;
		}
		*/

		// Vertices:
		for (int j = 0; j < 3; ++j)
		{
			// Create the vertex.
			Vertex v;
			Vert* current = t.vertices[j];
			v.setPosition((float)current->x, (float)current->y, (float)current->z);	
			v.setColor(color.x, color.y, color.z, 1.0f);
			v.setNormal((float)current->normal.x, (float)current->normal.y, (float)current->normal.z);
			v.setTexture(0, 0);
			v.setHighlightColor(glm::vec4(0, 0, 0, 1));

			if (horizon == max)
			{
				std::cout << "Vertex: " << i << std::endl;
				std::cout << glm::to_string(v.getPosition()) << std::endl;
				std::cout << glm::to_string(v.getNormal()) << std::endl;
			}

			// Barycentric coordinate for wireframe shader.
			glm::vec3 barycentric;
			if (j == 0)
				barycentric = glm::vec3(1, 0, 0);
			else if (j == 1)
				barycentric = glm::vec3(0, 1, 0);
			else
				barycentric = glm::vec3(0, 0, 1);
			v.setBarycentricCoordinate(barycentric);

			// Add to vertex list and triangle list.
			triangles.push_back(vertices.size());
			vertices.push_back(v);
		}
	}

	std::cout << "The number of triangles that diverge are " << divergeCount << std::endl;

	/*
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		Vertex v;
		Vert& current = p->vlist[i];

		v.setPosition((float)current.x, (float)current.y, (float)current.z);	
		v.setColor(color.x, color.y, color.z, 1.0f);
		v.setNormal((float)current.normal.x, (float)current.normal.y, (float)current.normal.z);
		v.setTexture(0, 0);
		vertices.push_back(v);
	}
	*/

	this->vertices = vertices;
	this->triangles = triangles;
	this->transform = glm::mat4(1);
}

// Blind copy.
MeshComponent::MeshComponent(Polyhedron *p)
{
	// Go through the Polyhedron's vlist and tlist and convert them to the correct data.
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

	glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	for (int i = 0; i < p->vlist.size(); ++i)
	{
		Vertex v;
		Vert& current = p->vlist[i];

		v.setPosition((float)current.x, (float)current.y, (float)current.z);	
		v.setColor(color.x, color.y, color.z, 1.0f);
		v.setNormal((float)current.normal.x, (float)current.normal.y, (float)current.normal.z);
		v.setTexture(0, 0);
		vertices.push_back(v);
	}

	for (int i = 0; i < p->tlist.size(); ++i)
	{
		Triangle& t = p->tlist[i];
		for (int j = 0; j < 3; ++j)
		{
			triangles.push_back(t.vertices[j]->index);
		}
	}

	this->vertices = vertices;
	this->triangles = triangles;
	this->transform = glm::mat4(1);
}

uint MeshComponent::getVBO()
{
	return vboID;
}
uint MeshComponent::getVAO()
{
	return vaoID;
}
uint MeshComponent::getCount()
{
	return triangles.size();
}
void MeshComponent::setVBO(uint vboID)
{
	this->vboID = vboID;
}
void MeshComponent::setVAO(uint vaoID)
{
	this->vaoID = vaoID;
}

std::vector<Vertex>& MeshComponent::getVertices()
{
	return vertices;
}
std::vector<uint>& MeshComponent::getTriangles()
{
	return triangles;
}

void MeshComponent::CreateModel(std::vector<Vertex> vertices, std::vector<uint> triangles)
{
	this->vertices = vertices;
	this->triangles = triangles;
}
