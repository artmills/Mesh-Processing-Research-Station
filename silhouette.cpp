#include "silhouette.hpp"

// Go through the triangle list of the polyhedron associated with the viewing direction.
// Compute dot products with the normal vectors of the triangles and the viewing direction.
// If the dot product is negative, the face is forward facing and is assigned the value TRUE.
// Otherwise, assign FALSE.
std::map<Triangle*, bool> Silhouette::GetFrontFacingTriangles(View& view)
{
	std::map<Triangle*, bool> frontTriangles;
	for (Triangle& t : view.p->tlist)
	{
		float dot = glm::dot(view.getViewDirection(), t.normal);
		if (dot < 0)
		{
			frontTriangles[&t] = true;
		}
		else
		{
			frontTriangles[&t] = false;
		}
	}
	return frontTriangles;
}

std::vector<Vert*> Silhouette::GetSilhouetteEdgesFromFaces(View& view)
{
	// Get the map from GetFrontFacingTriangles().
	// Iterate through the edge list and pick out the ones that connected a front-facing triangle to a back-facing one.
	std::vector<Vert*> silhouetteEdges;
	std::map<Triangle*, bool> frontTriangles = GetFrontFacingTriangles(view);
	for (Edge& e : view.p->elist)
	{
		// Silhouette:
		if (frontTriangles[e.triangles[0]] != frontTriangles[e.triangles[1]])
		{
			silhouetteEdges.push_back(e.vertices[0]);
			silhouetteEdges.push_back(e.vertices[1]);
		}
	}
	return silhouetteEdges;
}

std::map<Vert*, bool> Silhouette::GetFrontFacingVertices(View& view)
{
	std::map<Vert*, bool> frontVertices;
	for (Vert& v : view.p->vlist)
	{
		float dot = glm::dot(view.getViewDirection(), v.normal);
		if (dot < 0)
		{
			frontVertices[&v] = true;
		}
		else
		{
			frontVertices[&v] = false;
		}
	}
	return frontVertices;
}

glm::dvec3 Silhouette::LinearInterpolateZeroSet(Vert* v0, Vert* v1, double y0, double y1)
{
	glm::dvec3 x0 = glm::dvec3(v0->x, v0->y, v0->z);
	glm::dvec3 x1 = glm::dvec3(v1->x, v1->y, v1->z);
	double z0 = -y1 / (y0 - y1);
	double z1 = -y0 / (y1 - y0);
	return z0 * x0 + z1 * x1;
}

std::vector<glm::dvec3> Silhouette::GetSilhouetteEdgesFromVertices(View& view)
{
	// Get the map from GetFrontFacingVertices().
	// Iterate through the triangle list and consider all three edges.
	// If an edge has endpoints with opposite parity, pick out the point.
	// We are guarantted that if one edge has this, another edge will as well.
	// Push both of these points from the two edges -- this forms a new edge that will be part of the silhouette.
	std::vector<glm::dvec3> silhouetteEdges;
	std::map<Vert*, bool> frontVertices = GetFrontFacingVertices(view);

	for (Triangle& t : view.p->tlist)
	{
		for (Edge* e : t.edges)
		{
			// Silhouette:
			if (frontVertices[e->vertices[0]] != frontVertices[e->vertices[1]])
			{
				// Create a new vertex between these two vertices with a zero dot product.
				Vert* v0 = e->vertices[0];
				Vert* v1 = e->vertices[1];
				double y0 = glm::dot(view.getViewDirection(), v0->normal);
				double y1 = glm::dot(view.getViewDirection(), v1->normal);
				glm::dvec3 z = LinearInterpolateZeroSet(v0, v1, y0, y1);

				silhouetteEdges.push_back(z);
				/*
				if (frontVertices[e.vertices[0]])
				{
					v = glm::dvec3(e.vertices[0]->x, e.vertices[0]->y, e.vertices[0]->z);
				}
				else
				{
					v = glm::dvec3(e.vertices[1]->x, e.vertices[1]->y, e.vertices[1]->z);
				}
				silhouetteEdges.push_back(v);
				*/
			}
		}
	}

	return silhouetteEdges;
}

Silhouette::Silhouette() {}
Silhouette::~Silhouette() {}
