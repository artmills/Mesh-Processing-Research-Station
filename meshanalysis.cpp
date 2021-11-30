#include "meshanalysis.hpp"


MeshAnalysis::MeshAnalysis() {}
MeshAnalysis::~MeshAnalysis() {}

std::vector<double> MeshAnalysis::GetHorizonMeasuresDouble(std::vector<Triangle>& triangles)
{
	std::vector<double> horizonMeasures(triangles.size());
	for (int i = 0; i < triangles.size(); ++i)
	{
		horizonMeasures[i] = ComputeHorizonMeasureDouble(triangles[i]);
	}
	return horizonMeasures;
}
std::vector<float> MeshAnalysis::GetHorizonMeasures(std::vector<Triangle>& triangles)
{
	std::vector<float> horizonMeasures(triangles.size());
	for (int i = 0; i < triangles.size(); ++i)
	{
		horizonMeasures[i] = ComputeHorizonMeasure(triangles[i]);
	}
	return horizonMeasures;
}


float MeshAnalysis::ComputeHorizonMeasure(Vertex& v0, Vertex& v1, Vertex& v2)
{
	return ComputeHorizonArea(v0, v1, v2) / ComputePerimeter(v0, v1, v2);
}
float MeshAnalysis::ComputeHorizonMeasure(Triangle& t)
{
	return ComputeHorizonArea(t) / ComputePerimeter(t);
}
double MeshAnalysis::ComputeHorizonMeasureDouble(Triangle& t)
{
	return ComputeHorizonAreaDouble(t) / ComputePerimeterDouble(t);
}

float MeshAnalysis::ComputeHorizonArea(Vertex& v0, Vertex& v1, Vertex& v2)
{
	// Get dot products of the NORMAL vectors.
	// Clamp between -1 and -1 to avoid precision errors.
	float d01 = glm::clamp(glm::dot(v0.getNormal(), v1.getNormal()), -1.0f, 1.0f);
	float d12 = glm::clamp(glm::dot(v1.getNormal(), v2.getNormal()), -1.0f, 1.0f);
	float d20 = glm::clamp(glm::dot(v2.getNormal(), v0.getNormal()), -1.0f, 1.0f);

	// Take 2 times the sum of the inverse cosines.
	return 2.0f * (acos(d01) + acos(d12) + acos(d20));
}
double MeshAnalysis::ComputeHorizonAreaDouble(Triangle& t)
{
	// Get normal vectors.
	glm::dvec3 n0 = t.vertices[0]->normal;
	glm::dvec3 n1 = t.vertices[1]->normal;
	glm::dvec3 n2 = t.vertices[2]->normal;

	// Get dot products of the NORMAL vectors.
	// Clamp between -1 and -1 to avoid precision errors.
	double d01 = glm::clamp(glm::dot(n0, n1), -1.0, 1.0);
	double d12 = glm::clamp(glm::dot(n1, n2), -1.0, 1.0);
	double d20 = glm::clamp(glm::dot(n2, n0), -1.0, 1.0);

	// Take 2 times the sum of the inverse cosines.
	return 2.0 * (acos(d01) + acos(d12) + acos(d20));
}
float MeshAnalysis::ComputeHorizonArea(Triangle& t)
{
	// Get normal vectors.
	glm::vec3 n0 = t.vertices[0]->normal;
	glm::vec3 n1 = t.vertices[1]->normal;
	glm::vec3 n2 = t.vertices[2]->normal;

	// Get dot products of the NORMAL vectors.
	// Clamp between -1 and -1 to avoid precision errors.
	float d01 = glm::clamp(glm::dot(n0, n1), -1.0f, 1.0f);
	float d12 = glm::clamp(glm::dot(n1, n2), -1.0f, 1.0f);
	float d20 = glm::clamp(glm::dot(n2, n0), -1.0f, 1.0f);

	// Take 2 times the sum of the inverse cosines.
	return 2.0f * (acos(d01) + acos(d12) + acos(d20));
}

float MeshAnalysis::ComputePerimeter(Vertex& v0, Vertex& v1, Vertex& v2)
{
	// Get position vectors between the vertices.
	glm::vec3 p01 = v0.getPosition() - v1.getPosition();
	glm::vec3 p12 = v1.getPosition() - v2.getPosition();
	glm::vec3 p20 = v2.getPosition() - v0.getPosition();

	// Get lengths of these vectors.
	float l01 = glm::length(p01);
	float l12 = glm::length(p12);
	float l20 = glm::length(p20);

	// Return the sum.
	return l01 + l12 + l20;
}

double MeshAnalysis::ComputePerimeterDouble(Triangle& t)
{
	// Get the vertices of the triangle.
	glm::dvec3 v0 = glm::dvec3(t.vertices[0]->x, t.vertices[0]->y, t.vertices[0]->z);
	glm::dvec3 v1 = glm::dvec3(t.vertices[1]->x, t.vertices[1]->y, t.vertices[1]->z);
	glm::dvec3 v2 = glm::dvec3(t.vertices[2]->x, t.vertices[2]->y, t.vertices[2]->z);

	// Get position vectors between the vertices.
	glm::dvec3 p01 = v0 - v1;
	glm::dvec3 p12 = v1 - v2;
	glm::dvec3 p20 = v2 - v0;

	// Get lengths of these vectors.
	double l01 = glm::length(p01);
	double l12 = glm::length(p12);
	double l20 = glm::length(p20);

	// Return the sum.
	return l01 + l12 + l20;
}
float MeshAnalysis::ComputePerimeter(Triangle& t)
{
	// Get the vertices of the triangle.
	glm::vec3 v0 = glm::vec3(t.vertices[0]->x, t.vertices[0]->y, t.vertices[0]->z);
	glm::vec3 v1 = glm::vec3(t.vertices[1]->x, t.vertices[1]->y, t.vertices[1]->z);
	glm::vec3 v2 = glm::vec3(t.vertices[2]->x, t.vertices[2]->y, t.vertices[2]->z);

	// Get position vectors between the vertices.
	glm::vec3 p01 = v0 - v1;
	glm::vec3 p12 = v1 - v2;
	glm::vec3 p20 = v2 - v0;

	// Get lengths of these vectors.
	float l01 = glm::length(p01);
	float l12 = glm::length(p12);
	float l20 = glm::length(p20);

	// Return the sum.
	return l01 + l12 + l20;
}






void MeshAnalysis::GetCornerList(Polyhedron* p)
{
	std::vector<Triangle>& tlist = p->tlist;
	int numTriangles = tlist.size();
	std::vector<Corner>& corners = p->clist;

	// Analyze one triangle at a time.
	int cornerIndex = 0;
	for (int i = 0; i < numTriangles; ++i)
	{
		// Get the triangle and its vertices.
		Triangle& t = tlist[i];
		Vert* v1 = t.vertices[0];
		Vert* v2 = t.vertices[1];
		Vert* v3 = t.vertices[2];

		// Corners.
		Corner& c1 = corners[cornerIndex];
		c1.index = cornerIndex;
		Corner& c2 = corners[cornerIndex + 1];
		c2.index = cornerIndex + 1;
		Corner& c3 = corners[cornerIndex + 2];
		c3.index = cornerIndex + 2;
		 
		// Triangle: c.t.
		c1.t = &t;
		c2.t = &t;
		c3.t = &t;

		// Vertex: c.v.
		c1.v = v1;
		c2.v = v2;
		c3.v = v3;

		// Next: c.n.
		c1.n = &c2;
		c2.n = &c3;
		c3.n = &c1;

		// Previous: c.p.
		c1.p = &c3;
		c2.p = &c1;
		c3.p = &c2;

		// Edge: c.e.
		for (int j = 0; j < 3; ++j)
		{
			// For each vertex, check to see if that vertex is NOT contained in an edge.
			// The edge that does not contain the vertex is the edge that we want for c.e.
			Edge* e = t.edges[j];
			if (e->Contains(v1) == -1)
			{
				c1.e = e;
			}
			if (e->Contains(v2) == -1)
			{
				c2.e = e;
			}
			if (e->Contains(v3) == -1)
			{
				c3.e = e;
			}
		}

		// And now the hard part...
		// Opposite: c.o.
		for (int j = 0; j < 3; ++j)
		{
			Corner& c = corners[cornerIndex + j];

			// Get the edge of the corner and find the triangle that is not equal to c.t.
			Edge* e = c.e;

			// If this edge is attached to only one triangle then there is no opposite corner.
			if (e->numberOfTriangles > 1)
			{
				// Pick the correct triangle that is NOT equal to the current triangle c.t.	
				Triangle* s = e->GetOtherTriangle(&t);

				// Look through the vertices of the triangle and find the one that is not in the shared edge.
				for (int k = 0; k < 3; ++k)
				{
					int triangleVertexIndex = k;
					Vert* w = s->vertices[triangleVertexIndex];
					if (e->Contains(w) == -1)
					{
						// The annoying part.
						// We need to figure out what the index of the desired corner c.o will be, before it is even initialized.
						// Since we are creating these corners by iterating through the triangles, we can determine the future index of c.o.
						// First, each triangle generates three corners.
						// So if we are looking at the ith triangle, then the desired corner will have index 3i, 3i+1, or 3i+2.
						int index = 3 * s->index + triangleVertexIndex;
						c.o = &corners[index];
						//c.Print();
						break;
					}
				}
			}
			else
			{
				c.o = NULL;
			}
		}

		cornerIndex += 3;
	}
}

void MeshAnalysis::GetValenceDeficit(Polyhedron* p)
{
	// We can cycle around a vertex using corners.
	// Given a corner, check c.p.o.p to get a new corner at the same vertex.
	// Hop around in this fashion until we get back to where we started.

	// This method requires that the polyhedron has a corner table.
	if (p->clist.size() < 1)
	{
		GetCornerList(p);
	}
	std::vector<Corner>& corners = p->clist;

	for (Corner c : corners)
	{
		// Valence:
		// Don't bother computing if the valence of this vertex has already been set.
		if (c.v->valence == 0)
		{
			// Bounce around until we get back to where we started.
			int index = -1;
			int valence = 0;
			Corner* previous = &c;
			while (index != c.index)
			{
				// Valence computation:
				Corner* adjacent = previous->p->o->p;
				index = adjacent->index;

				valence++;
				previous = adjacent;
			}
			c.v->valence = valence;
			int deficit = 6 - valence;
			p->valenceDeficit += deficit;
		}
	}
}

void MeshAnalysis::ComputeAngle(Corner& c)
{
	glm::dvec3 cWorld = glm::dvec3(c.v->x, c.v->y, c.v->z);
	glm::dvec3 cnWorld = glm::dvec3(c.n->v->x, c.n->v->y, c.n->v->z);
	glm::dvec3 cpWorld = glm::dvec3(c.p->v->x, c.p->v->y, c.p->v->z);
	glm::dvec3 cn = cnWorld - cWorld;
	glm::dvec3 cp = cpWorld - cWorld;

	double dot = glm::dot(cn, cp);
	double cnLength = glm::length(cn);
	double cpLength = glm::length(cp);
	double angle = acos(dot / (cnLength * cpLength));
	c.angle = angle;
}

void MeshAnalysis::ComputeAngles(std::vector<Corner>& corners)
{
	for (Corner& c : corners)
	{
		// Check to see if the angle has already been computed. If not, continue:
		if (c.angle == 0)
		{
			ComputeAngle(c);

			// Add this angle to the vertex total angle.
			c.v->totalAngle += c.angle;

			// Add this angle to the corresponding triangle angle.
			// First get the index:
			//int k = c.t->Contains(c.v);
			//c.t->angles[k] = c.angle;
		}
	}
}

void MeshAnalysis::GetAngleDeficit(Polyhedron* p)
{
	ComputeAngles(p->clist);
	double totalAngleDeficit = 0;
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		double vertexAngle = p->vlist[i].totalAngle;
		//std::cout << vertexAngle << std::endl;
		double angleDeficit = 2 * M_PI - vertexAngle;
		totalAngleDeficit += angleDeficit;
	}
	p->angleDeficit = totalAngleDeficit;
}

float MeshAnalysis::InverseLerp(float start, float end, float v)
{
	return (v - start) / (end  - start);
}
