#include "linevertex.hpp"


LineVertex::LineVertex(glm::vec3 position, glm::vec3 color)
{
	x = position.x;
	y = position.y;
	z = position.z;
	r = color.r;
	g = color.g;
	b = color.b;
	a = 1.0f;
}
LineVertex::LineVertex(float x, float y, float z, 
					   float r, float g, float b)
{
	this->x = x;
	this->y = y;
	this->z = y;
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1.0f;
}
LineVertex::~LineVertex(){}

glm::vec3 LineVertex::GetPosition()
{
	return glm::vec3(this->x, this->y, this->z);
}
glm::vec4 LineVertex::GetColor()
{
	return glm::vec4(this->r, this->g, this->b, this->a);
}

void LineVertex::Print()
{
	std::cout << "Position: " << x << " " << y << " " << z << std::endl;
	std::cout << "Color: " << r << " " << g << " " << b << " " << a << std::endl;
	std::cout << std::endl;
}
