#pragma once

#include<map>
#include "polyhedron.hpp"
#include "view.hpp"


/** Given a polyhedron and a view object, construct the silhouette of the polyhedron as seen from the viewing position/direction.
 * Two ways to do construct a silhouette:
 *
 * 1) Dot products: classify faces as either forward-facing or backward-facring by computing dot products.
 * Then pick out the edges that separate forward and backward faces.
 *
 * 2) Face-based: do the dot product calculation again, but at vertices.
 * If an edge has vertices with different signs, interpolate within the edge to determine a point for which the dot product is exactly zero (exists by intermediate value theorem).
 * Assign this point a Vert object and connect the new vertex to the edge vertex with negative dot product.
 */
class Silhouette
{

public:

	// Use the first dot product method with triangle normals to get forward-facing triangles.
	static std::map<Triangle*, bool> GetFrontFacingTriangles(View& view);

	// Using the front-facing triangles, extract the silhouette edges.
	static std::vector<Vert*> GetSilhouetteEdgesFromFaces(View& view);

	// Use the second dot product method with vertex normals to get forward-facing vertices.
	static std::map<Vert*, bool> GetFrontFacingVertices(View& view);

	// Linearly interpolate between two given values to get a point for which the dot product is exactly zero.
	static glm::dvec3 LinearInterpolateZeroSet(Vert* v0, Vert* v1, double y0, double y1);

	// Using the front-facing vertices, extract the silhouette edges.
	static std::vector<glm::dvec3> GetSilhouetteEdgesFromVertices(View& view);


private:

	Silhouette();
	~Silhouette();


};
