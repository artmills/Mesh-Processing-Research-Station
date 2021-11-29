#include "vertex.hpp"

Vertex::Vertex() {}
Vertex::Vertex(float _x, float _y, float _z, float _r, float _g, float _b, float _a)
	: x(_x), y(_y), z(_z), r(_r), g(_g), b(_b), a(_a) {}
Vertex::Vertex(float _x, float _y, float _z, glm::vec4 color)
{
	this->x = _x;
	this->y = _y;
	this->z = _z;
	this->r = color.r;
	this->g = color.g;
	this->b = color.b;
	this->a = color.a;
}

void Vertex::setHighlightColor(glm::vec4 color)
{
	this->h1 = color.r;
	this->h2 = color.g;
	this->h3 = color.b;
	this->h4 = color.a;
}

void Vertex::setBarycentricCoordinate(glm::vec3 b)
{
	this->b1 = b.x;
	this->b2 = b.y;
	this->b3 = b.z;
}

glm::vec3 Vertex::getNormal()
{
	return glm::vec3(nx, ny, nz);
}
glm::vec3 Vertex::getPosition()
{
	return glm::vec3(x, y, z);
}

void Vertex::setColor(glm::vec4 color)
{
	this->r = color.r;
	this->g = color.g;
	this->b = color.b;
	this->a = color.a;
}
void Vertex::setPosition(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
void Vertex::setColor(float r, float g, float b, float a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}
void Vertex::setNormal(float nx, float ny, float nz)
{
	this->nx = nx;
	this->ny = ny;
	this->nz = nz;
}
void Vertex::setTexture(float s, float t)
{
	this->s = s;
	this->t = t;
}

void Vertex::PrintPosition()
{
	std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
}
