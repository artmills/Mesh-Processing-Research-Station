#include "curvecomponent.hpp"
#include <limits>

CurveComponent::CurveComponent()
{
	this->transform = glm::mat4(1);
}
CurveComponent::CurveComponent(std::vector<LineVertex> vertices)
{
	this->vertices = vertices;
	this->transform = glm::mat4(1);
}

double CurveComponent::InverseLerp(double start, double end, double v)
{
	return (v - start) / (end  - start);
}
float CurveComponent::InverseLerp(float start, float end, float v)
{
	return (v - start) / (end  - start);
}
glm::vec3 CurveComponent::Lerp(glm::vec3 start, glm::vec3 end, float t)
{
	return ((1.0f - t) * start + t * end);
}

uint CurveComponent::getVBO()
{
	return vboID;
}
uint CurveComponent::getVAO()
{
	return vaoID;
}
uint CurveComponent::getCount()
{
	return vertices.size();
}
void CurveComponent::setVBO(uint vboID)
{
	this->vboID = vboID;
}
void CurveComponent::setVAO(uint vaoID)
{
	this->vaoID = vaoID;
}

std::vector<LineVertex>& CurveComponent::getVertices()
{
	return vertices;
}
