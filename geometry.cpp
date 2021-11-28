#include "geometry.hpp"


Vert::Vert() 
{
	this->normal = glm::dvec3(0.0, 0.0, 0.0);
}
Vert::Vert(double x, double y, double z) 
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->normal = glm::dvec3(0.0, 0.0, 0.0);
}
Vert::~Vert() {}
Vert::Vert(const Vert& v)
{
	this->index = v.index;
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->normal = v.normal;
	this->numberOfTriangles = v.numberOfTriangles;
	this->triangles = v.triangles;
	this->valence = v.valence;
	this->totalAngle = v.totalAngle;
	this->value0 = v.value0;
	this->value1 = v.value1;
	this->minMax = v.minMax;
	this->saddle = v.saddle;
}
Vert::Vert(Vert&& v)
{
	this->index = v.index;
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->normal = v.normal;
	this->numberOfTriangles = v.numberOfTriangles;
	this->triangles = v.triangles;
	this->valence = v.valence;
	this->totalAngle = v.totalAngle;
	this->value0 = v.value0;
	this->value1 = v.value1;
	this->minMax = v.minMax;
	this->saddle = v.saddle;
}
Vert& Vert::operator=(Vert&& v)
{
	this->index = v.index;
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->normal = v.normal;
	this->numberOfTriangles = v.numberOfTriangles;
	this->triangles = v.triangles;
	this->valence = v.valence;
	this->totalAngle = v.totalAngle;
	this->value0 = v.value0;
	this->value1 = v.value1;
	this->minMax = v.minMax;
	this->saddle = v.saddle;
	return *this;
}

void Vert::Print()
{
	std::cout << "Vert #" << this->index << ": ";
	std::cout << "( " << x  << ", " << y << ", " << z;
	std::cout << " ). ";
	std::cout << "Attached to " << numberOfTriangles << " triangles: ";
	for (int i = 0; i < numberOfTriangles; ++i)
	{
		std::cout << triangles[i]->index << " ";	
	}
	std::cout << "- Normal vector: ";
	std::cout << "( " << normal.x  << ", " << normal.y << ", " << normal.z;
	std::cout << " ). ";
	std::cout << std::endl;
}

Edge::Edge()
{
	this->vertices = std::vector<Vert*>(2);
}
Edge::~Edge() {}
Edge::Edge(const Edge& e)
{
	this->index = e.index;
	this->length = e.length;
	this->vertices = e.vertices;
	this->numberOfTriangles = e.numberOfTriangles;
	this->triangles = e.triangles;
}
Edge::Edge(Edge&& e)
{
	this->index = e.index;
	this->length = e.length;
	this->vertices = e.vertices;
	this->numberOfTriangles = e.numberOfTriangles;
	this->triangles = e.triangles;
}
Edge& Edge::operator=(Edge&& e)
{
	this->index = e.index;
	this->length = e.length;
	this->vertices = e.vertices;
	this->numberOfTriangles = e.numberOfTriangles;
	this->triangles = e.triangles;
	return *this;
}

void Edge::Print()
{
	std::cout << "Edge #" << index << ": ";
	std::cout << "Length: " << length << "; ";
	std::cout << "Attached to vertices: ";
	for (int i = 0; i < 2; ++i)
	{
		if (vertices[i])
			std::cout << vertices[i]->index << " ";	
	}
	std::cout << ". Attached to " << numberOfTriangles << " triangles: ";
	for (int i = 0; i < 2; ++i)
	{
		if (triangles[i] != NULL)
			std::cout << triangles[i]->index << " ";	
	}
	std::cout << std::endl;
}

bool Edge::isBoundary()
{
	return (numberOfTriangles == 1);
}

Triangle* Edge::GetOtherTriangle(Triangle* t)
{
	if (numberOfTriangles < 2)
	{
		return NULL;
	}
	if (t->index == triangles[0]->index)
	{
		return triangles[1];
	}
	else
	{
		return triangles[0];
	}

}

Vert* Edge::GetOtherVertex(Vert* v)
{
	if (v->index == vertices[0]->index)
	{
		return vertices[1];
	}
	else if (v->index == vertices[1]->index)
	{
		return vertices[0];
	}
	else
	{
		return NULL;
	}
}

void Edge::ComputeLength()
{
	glm::dvec3 v0 = glm::dvec3(vertices[0]->x, vertices[0]->y, vertices[0]->z);
	glm::dvec3 v1 = glm::dvec3(vertices[1]->x, vertices[1]->y, vertices[1]->z);
	length = glm::length(v1 - v0);	
}

int Edge::Contains(Vert* v)
{
	for (int i = 0; i < vertices.size(); ++i)
	{
		if (vertices[i]->index == v->index)
		{
			return i;
		}
	}
	return -1;
}

Triangle::Triangle()
{
	this->vertices = std::vector<Vert*>(3);
	this->edges = std::vector<Edge*>(3);
	this->normal = glm::dvec3(0.0, 0.0, 0.0);
}
Triangle::~Triangle() {}
Triangle::Triangle(const Triangle& t)
{
	this->index = t.index;
	this->normal = t.normal;
	this->area = t.area;
	this->vertices = t.vertices;
	this->edges = t.edges;
}
Triangle::Triangle(Triangle&& t)
{
	this->index = t.index;
	this->normal = t.normal;
	this->area = t.area;
	this->vertices = t.vertices;
	this->edges = t.edges;
}
Triangle& Triangle::operator=(Triangle&& t)
{
	this->index = t.index;
	this->normal = t.normal;
	this->area = t.area;
	this->vertices = t.vertices;
	this->edges = t.edges;
	return *this;
}

void Triangle::Print()
{
	std::cout << "Triangle #" << index << ": ";
	std::cout << "Attached to vertices: ";
	for (int i = 0; i < 3; ++i)
	{
		if (vertices[i])
			std::cout << vertices[i]->index << " ";	
	}
	std::cout << "- Attached to edges: ";
	for (int i = 0; i < 3; ++i)
	{
		if (edges[i])
			std::cout << edges[i]->index << " ";	
	}
	std::cout << "- Normal vector: ";
	std::cout << "( " << normal.x << ", " << normal.y << ", " << normal.z;
	std::cout << " ) ";
	std::cout << "- Area: " << area;
	std::cout << std::endl;
}

int Triangle::Contains(Vert* v)
{
	for (int i = 0; i < 3; ++i)
	{
		if (v->index == vertices[i]->index)
		{
			return i;
		}
	}
	return -1;
}

void Triangle::ComputeNormalAndArea()
{
	double halfPerimeter = 0.5 * (edges[0]->length + edges[1]->length + edges[2]->length);

	this->area = sqrt(halfPerimeter 
			* (halfPerimeter - edges[0]->length)
			* (halfPerimeter - edges[1]->length)
			* (halfPerimeter - edges[2]->length)
			);

	glm::dvec3 v1 = glm::dvec3(vertices[0]->x, vertices[0]->y, vertices[0]->z);
	glm::dvec3 v2 = glm::dvec3(vertices[1]->x, vertices[1]->y, vertices[1]->z);
	glm::dvec3 v0 = glm::dvec3(vertices[2]->x, vertices[2]->y, vertices[2]->z);
	this->normal = glm::normalize(glm::cross(v0 - v1, v2 - v1));


}


Corner::Corner() {}
Corner::~Corner() {}

std::vector<Vert*> Corner::GetAdjacentVertices()
{
	std::vector<Vert*> connected;
	// Bounce around until we get back to where we started.
	int k = -1;
	Corner* previous = this;
	while (k != this->index)
	{
		Corner* adjacent = previous->p->o->p;
		k = adjacent->index;

		connected.push_back(adjacent->n->v);
		previous = adjacent;
	}
	return connected;
}

void Corner::Print()
{
	std::cout << std::endl;
	std::cout << "Corner #" << index << std::endl;
	std::cout << "c.v: " << v->index << std::endl;
	std::cout << "c.e: " << e->index << std::endl;
	std::cout << "c.t: " << t->index << std::endl;
	std::cout << "c.n: " << n->index << std::endl;
	std::cout << "c.n.v: " << n->v->index << std::endl;
	std::cout << "c.p: " << p->index << std::endl;
	std::cout << "c.p.v: " << p->v->index << std::endl;
	if (o != NULL && o->index != -1)
	{
		std::cout << "c.o: " << o->index << std::endl;
		std::cout << "c.o.v: " << o->v->index << std::endl;
	}
	else
		std::cout << "c.o: NULL" << std::endl;
	std::cout << "Angle: " << angle << std::endl;
	std::cout << std::endl;
}

Corner::Corner(const Corner& c)
{
	this->index = c.index;
	this->v = c.v;
	this->e = c.e;
	this->t = c.t;
	this->n = c.n;
	this->p = c.p;
	this->o = c.o;
	this->angle = c.angle;
}
Corner::Corner(Corner&& c)
{
	this->index = c.index;
	this->v = c.v;
	this->e = c.e;
	this->t = c.t;
	this->n = c.n;
	this->p = c.p;
	this->o = c.o;
	this->angle = c.angle;
}
Corner& Corner::operator=(Corner&& c)
{
	this->index = c.index;
	this->v = c.v;
	this->e = c.e;
	this->t = c.t;
	this->n = c.n;
	this->p = c.p;
	this->o = c.o;
	this->angle = c.angle;
	return *this;
}

