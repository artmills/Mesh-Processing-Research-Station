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
float MeshComponent::InverseLerp(float start, float end, float v)
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

	// Compute statistics:
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
	double mean = sum / (double )triangleHorizon.size();

	double stdSums = 0;
	for (int i = 0; i < triangleHorizon.size(); ++i)
	{
		stdSums += (triangleHorizon[i] - mean) * (triangleHorizon[i] - mean);
	}
	double standardDeviation = sqrt((1.0f / (double)triangleHorizon.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Length(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;

	int divergeCount = 0;
	double maxDistanceFromMean = max - mean;
	if (mean - min > maxDistanceFromMean)
		maxDistanceFromMean = mean - min;

	int count = 0;

	// Get the vertices of each triangle.
	for (int i = 0; i < p->tlist.size(); ++i)
	{
		Triangle& t = p->tlist[i];
		

		// Look up horizon measure and compare it to the average.
		double horizon = triangleHorizon[i];

		color = InterpolateColor(min, mean, max, horizon);
		//color = InterpolateColorPercentage(min, mean, max, maxDistanceFromMean, horizon);

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
	std::cout << count << std::endl;

	this->vertices = vertices;
	this->triangles = triangles;
	this->transform = glm::mat4(1);
}

void MeshComponent::AssignHorizonMeasureColors(std::vector<float>& triangleHorizon)
{
	glm::vec3 color;

	// Compute statistics:
	float sum = 0;
	float max = 0;
	float min = std::numeric_limits<float>::max();
	for (int i = 0; i < triangleHorizon.size(); ++i)
	{
		float x = triangleHorizon[i];
		sum += x;	
		if (x > max)
		{
			max = x;
		}
		if (x < min)
			min = x;
	}
	float mean = sum / (float)triangleHorizon.size();

	float stdSums = 0;
	for (int i = 0; i < triangleHorizon.size(); ++i)
	{
		stdSums += (triangleHorizon[i] - mean) * (triangleHorizon[i] - mean);
	}
	float standardDeviation = sqrt((1.0f / (float )triangleHorizon.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Length(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;

	float maxDistanceFromMean = max - mean;
	if (mean - min > maxDistanceFromMean)
		maxDistanceFromMean = mean - min;

	// Get the vertices of each triangle.
	for (int i = 0; i < triangleHorizon.size(); ++i)
	{
		// Look up horizon measure and compare it to the average.
		float horizon = triangleHorizon[i];
		color = InterpolateColor(min, mean, max, horizon);

		/*
		float distFromMean = std::abs(horizon - mean);
		float percent = InverseLerp(0, maxDistanceFromMean, distFromMean);
		percent = cbrt(percent);
		color = glm::vec3(1.0f, 1.0f - percent, 1.0f - percent);
		*/

		vertices[3*i].setColor(color.x, color.y, color.z, 1.0f);
		vertices[3*i].setBarycentricCoordinate(glm::vec3(1, 0, 0));

		vertices[3*i+1].setColor(color.x, color.y, color.z, 1.0f);
		vertices[3*i+1].setBarycentricCoordinate(glm::vec3(0, 1, 0));

		vertices[3*i+2].setColor(color.x, color.y, color.z, 1.0f);
		vertices[3*i+2].setBarycentricCoordinate(glm::vec3(0, 0, 1));
	}
}



glm::vec3 MeshComponent::InterpolateColorPercentage(float min, float mean, float max, float maxDistanceFromMean, float value)
{
	float distFromMean = std::abs(value - mean);
	float percent = InverseLerp(0, maxDistanceFromMean, distFromMean);
	percent = cbrt(percent);
	glm::vec3 color = glm::vec3(1.0f, 1.0f - percent, 1.0f - percent);
	/*
	if (horizon > 0.5f * mean && horizon > 1.5f * mean)
	{
		color = glm::vec3(1.0f, 1.0f - percent, 1.0f - percent);
		++count;
	}
	else
		color = glm::vec3(1, 1, 1);
	*/
	return color;
}
glm::vec3 MeshComponent::InterpolateColorPercentage(double min, double mean, double max, double maxDistanceFromMean, double value)
{
	double distFromMean = std::abs(value - mean);
	double percent = InverseLerp(0, maxDistanceFromMean, distFromMean);
	percent = cbrt(percent);
	glm::vec3 color = glm::vec3(1.0f, 1.0f - percent, 1.0f - percent);
	/*
	if (horizon > 0.5f * mean && horizon > 1.5f * mean)
	{
		color = glm::vec3(1.0f, 1.0f - percent, 1.0f - percent);
		++count;
	}
	else
		color = glm::vec3(1, 1, 1);
	*/
	return color;
}

glm::vec3 MeshComponent::InterpolateColor(double min, double mean, double max, double value)
{
	glm::vec3 color;

	// min <= value < mean:
	if (value >= min && value < mean)
	{
		double percent = InverseLerp(min, mean, value);
		//percent *= percent * percent;
		color = glm::vec3(0, percent, 1.0f - percent);
	}
	// mean < value <= max:
	else if (value > mean && value <= max)
	{
		double percent = InverseLerp(mean, max, value);
		percent = cbrt(percent);
		color = glm::vec3(percent, 1.0f - percent, 0);
	}
	// value = mean:
	else if (value == mean)
	{
		color = glm::vec3(0, 1.0f, 0);	
	}
	else
	{
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		std::cout << "Error! " << min << " " << mean << " " << max << " " << value << std::endl;
	}
	return color;
}
glm::vec3 MeshComponent::InterpolateColor(float min, float mean, float max, float value)
{
	glm::vec3 color;

	// min <= value < mean:
	if (value >= min && value < mean)
	{
		float percent = InverseLerp(min, mean, value);
		//percent *= percent * percent;
		color = glm::vec3(0, percent, 1.0f - percent);
	}
	// mean < value <= max:
	else if (value > mean && value <= max)
	{
		float percent = InverseLerp(mean, max, value);
		percent = cbrt(percent);
		color = glm::vec3(percent, 1.0f - percent, 0);
	}
	// value = mean:
	else if (value == mean)
	{
		color = glm::vec3(0, 1.0f, 0);	
	}
	else
	{
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		std::cout << "Error! " << min << " " << mean << " " << max << " " << value << std::endl;
	}
	return color;
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
