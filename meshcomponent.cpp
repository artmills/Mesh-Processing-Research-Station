#include "meshcomponent.hpp"
#include <limits>

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
glm::vec3 MeshComponent::Lerp(glm::vec3 start, glm::vec3 end, float t)
{
	return ((1.0f - t) * start + t * end);
}

glm::vec4 MeshComponent::InterpolateColor(double min, double max, double mean, double value)
{
	glm::vec3 maxColor = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 meanColor = glm::vec3(0.0f, 0.8f, 0.0f);
	glm::vec3 minColor = glm::vec3(0.0f, 0.0f, 1.0f);

	//std::cout << min << " " << max << " " << mean << " " << value << std::endl;
	
	glm::vec3 color;
	if (value > mean)
	{
		float percent = (float)InverseLerp(mean, max, value);
		color = Lerp(meanColor, maxColor, percent);
	}
	else if (value < mean)
	{
		float percent = (float)InverseLerp(min, mean, value);
		color = Lerp(minColor, meanColor, percent);
	}

	return glm::vec4(color, 1.0f);
}

glm::vec4 MeshComponent::InterpolateSignedColor(double minNegative, double maxPositive, double meanNegative, double meanPositive, double value)
{
	// If value > 0, then interpolate between max{0, min} and meanPositive.
	// If value < 0, then interpolate between meanNegative and min{0, max}.
	// For values above/below the mean, just set to the max color value.
	glm::vec3 positive = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 zero = glm::vec3(0.0f, 0.8f, 0.0f);
	glm::vec3 negative = glm::vec3(0.0f, 0.0f, 1.0f);
	
	// Also if value > 0, this means that max > 0 also.
	glm::vec3 color;
	if (value > 0.0)
	{
		// If the value is greater than the mean, just set the color to max.
		if (value >= meanPositive)
			color = positive;
		else
		{
			double minBound = (0.0 > minNegative ? 0.0 : minNegative);
			float percent = (float)(InverseLerp(minBound, meanPositive, value));
			color = Lerp(positive, zero, percent);
			//std::cout << value << " " << maxPositive << " " << meanPositive << " " << percent << " " << glm::to_string(color) << std::endl;
		}
		return glm::vec4(color, 1.0f);
	}
	else if (value < 0.0)
	{
		// If the value is less than the mean, just set the color to min.
		if (value <= meanNegative)
			color = negative;
		else
		{
			double maxBound = (0.0 < maxPositive ? 0.0 : maxPositive);
			float percent = (float)(InverseLerp(meanNegative, maxBound, value));
			percent *= percent;
			color = Lerp(negative, zero, percent);
		}
		return glm::vec4(color, 1.0f);
	}
	else
	{
		return glm::vec4(zero, 1.0f);
	}
}

// Vertex-based mesh coloring.
MeshComponent::MeshComponent(Polyhedron* p, std::vector<double>& values, Curvature c)
{
	// Convert the Polyhedron's vlist and tlist to the correct rendering data.
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

	// Compute statistics of the values for coloring.
	double minNegative = std::numeric_limits<double>::max();
	double maxPositive = std::numeric_limits<double>::min();
	double meanNegative = 0;
	double meanPositive = 0;
	int countNegative = 0;
	int countPositive = 0;
	for (int i = 0; i < values.size(); ++i)
	{
		double value = values[i];
		if (value < 0)
		{
			meanNegative += value;
			countNegative += 1;
			if (value < minNegative)
				minNegative = value;
		}
		else if (value > 0)
		{
			meanPositive += value;
			countPositive += 1;
			if (value > maxPositive)
				maxPositive = value;
		}
	}
	meanNegative /= (double)countNegative;
	meanPositive /= (double)countPositive;

	std::cout << "MEAN NEGATIVE: " << meanNegative << std::endl;
	std::cout << "MEAN POSITIVE: " << meanPositive << std::endl;
	std::cout << "MIN NEGATIVE: " << minNegative << std::endl;
	std::cout << "MAX POSITIVE: " << maxPositive << std::endl;


	// Build up list of vertices.
	for (Vert& current : p->vlist)
	{
		// Create the vertex.
		Vertex v;
		v.setPosition((float)current.x, (float)current.y, (float)current.z);
		v.setNormal((float)current.normal.x, (float)current.normal.y, (float)current.normal.z);
		v.setTexture(0, 0);
		v.setHighlightColor(glm::vec4(0, 0, 0, 1));
		v.setBarycentricCoordinate(glm::vec3(0, 0, 0));

		int index = current.index;
		if (c == Curvature::GAUSSIAN || c == Curvature::DISTORTION_SIGNED || c == Curvature::MEAN_SIGNED)
			v.setColor(InterpolateSignedColor(minNegative, maxPositive, meanNegative, meanPositive, values[index]));
			//v.setColor(InterpolateColor(0, maxPositive, meanPositive, values[index]));
		else
			v.setColor(InterpolateColor(0, maxPositive, meanPositive, values[index]));
		/*
		if (values[index] < 0.001f)
			v.setColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		else
			v.setColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		*/

		vertices.push_back(v);
	}

	// Stitch together triangles.
	for (Triangle& t : p->tlist)
	{
		triangles.push_back(t.vertices[0]->index);
		triangles.push_back(t.vertices[1]->index);
		triangles.push_back(t.vertices[2]->index);
	}

	this->vertices = vertices;
	this->triangles = triangles;
	this->transform = glm::mat4(1);
}

// Blind copy.
MeshComponent::MeshComponent(Polyhedron *p, glm::vec3 color)
{
	// Go through the Polyhedron's vlist and tlist and convert them to the correct data.
	std::vector<Vertex> vertices;
	std::vector<uint> triangles;

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
