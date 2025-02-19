#include "subdivision.hpp"

Polyhedron* Subdivision::LoopSubdivisionHeap(Polyhedron* p)
{
	int originalVertices = p->vlist.size();
	int originalEdges = p->elist.size();
	int originalFaces = p->tlist.size();

	Polyhedron* loop = new Polyhedron(originalVertices + originalEdges, 3 * originalFaces + 2 * originalEdges, 4 * originalFaces);

	// Map that stores (edge.index, vert):
	std::map<int, Vert*> oddVertices;

	/** Even vertices:
	 * We will compute the even vertices using the original vertices of the mesh.
	 * These vertices are exact copies of the originals, but their positions are adjusted according to the Loop scheme.
	 */
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		Vert even = CreateEvenVertex(&p->vlist[i]);
		loop->vlist.push_back(even);
	}
	int numberOfEvenVertices = p->vlist.size();

	/** Odd vertices:
	 * We will compute the odd vertices using the edges of the mesh.
	 * The new vertices will be stored in a map structure so we can keep track of the new (edge, vertex) pairs that are created.
	 */
	int count = numberOfEvenVertices;
	for (int i  = 0; i < p->elist.size(); ++i)
	{
		// Use the edge to create the odd vertex.
		Edge* e = &p->elist[i];
		Vert odd = CreateOddVertex(e, count);

		// Add the odd vertex to both the map and the vlist.
		loop->vlist.push_back(odd);
		oddVertices.insert({ e->index, &loop->vlist[loop->vlist.size() - 1] });
		count++;
	}

	/** Topology:
	* And now for the hard part!
	* Go through each triangle of the original mesh.
	* Look up the odd vertices for each of the edges in the given triangle.
	* Since the evenVertices list was created by looping through the vlist, we are guaranteed that the indices match between lists.
	* Therefore we may do all of our work using the evenVertices list instead of vlist.
	* We then can carefully construct the four triangles.
	*/ 
	for (int i = 0; i < p->tlist.size(); ++i)
	{
		// Start with a triangle:
		Triangle* t = &p->tlist[i];

		//std::cout << "Creating vertices!" << std::endl;
		// Even vertices:
		Vert* v0 = &loop->vlist[t->vertices[0]->index];
		Vert* v1 = &loop->vlist[t->vertices[1]->index];
		Vert* v2 = &loop->vlist[t->vertices[2]->index];

		// Odd vertices:
		int w0Index = oddVertices[t->edges[0]->index]->index;
		int w1Index = oddVertices[t->edges[1]->index]->index;
		int w2Index = oddVertices[t->edges[2]->index]->index;
		Vert* w0 = &loop->vlist[w0Index];
		Vert* w1 = &loop->vlist[w1Index];
		Vert* w2 = &loop->vlist[w2Index];

		int tIndex = loop->tlist.size();

		//std::cout << "Creating triangles: " << std::endl;
		//std::cout << "Decomposing triangle: " << t->index << ": " << t->verts[0]->index << " " << t->verts[1]->index << " " << t->verts[2]->index << std::endl;

		Triangle t1;
		t1.index = tIndex;
		t1.vertices[0] = v0;
		t1.vertices[1] = w0;
		t1.vertices[2] = w2;
		//std::cout << "First triangle: " << v0->index << " " << w0->index << " " << w2->index << std::endl;

		Triangle t2;
		t2.index = tIndex + 1;
		t2.vertices[0] = w0;
		t2.vertices[1] = v1;
		t2.vertices[2] = w1;
		//std::cout << "Second triangle: " << w0->index << " " << v1->index << " " << w1->index << std::endl;

		Triangle t3;
		t3.index = tIndex + 2;
		t3.vertices[0] = w1;
		t3.vertices[1] = v2;
		t3.vertices[2] = w2;
		//std::cout << "Third triangle: " << w1->index << " " << v2->index << " " << w2->index << std::endl;

		Triangle t4;
		t4.index = tIndex + 3;
		t4.vertices[0] = w0;
		t4.vertices[1] = w1;
		t4.vertices[2] = w2;
		//std::cout << "Fourth triangle: " << w0->index << " " << w1->index << " " << w2->index << std::endl;
		//std::cout << std::endl;

		loop->tlist.push_back(t1);
		loop->tlist.push_back(t2);
		loop->tlist.push_back(t3);
		loop->tlist.push_back(t4);
	}

	// Don't forget to initialize the clist!
	Corner c;
	loop->clist = std::vector<Corner>(3 * loop->tlist.size(), c);
	return loop;
}

Polyhedron Subdivision::LoopSubdivision(Polyhedron* p)
{
	Polyhedron loop;

	// Map that stores (edge.index, vert):
	std::map<int, Vert*> oddVertices;

	/** Even vertices:
	 * We will compute the even vertices using the original vertices of the mesh.
	 * These vertices are exact copies of the originals, but their positions are adjusted according to the Loop scheme.
	 */
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		Vert even = CreateEvenVertex(&p->vlist[i]);
		loop.vlist.push_back(even);
	}
	int numberOfEvenVertices = p->vlist.size();

	/** Odd vertices:
	 * We will compute the odd vertices using the edges of the mesh.
	 * The new vertices will be stored in a map structure so we can keep track of the new (edge, vertex) pairs that are created.
	 */
	int count = numberOfEvenVertices;
	for (int i  = 0; i < p->elist.size(); ++i)
	{
		// Use the edge to create the odd vertex.
		Edge* e = &p->elist[i];
		Vert odd = CreateOddVertex(e, count);

		// Add the odd vertex to both the map and the vlist.
		loop.vlist.push_back(odd);
		oddVertices.insert({ e->index, &loop.vlist[loop.vlist.size() - 1] });
		count++;
	}

	/** Topology:
	* And now for the hard part!
	* Go through each triangle of the original mesh.
	* Look up the odd vertices for each of the edges in the given triangle.
	* Since the evenVertices list was created by looping through the vlist, we are guaranteed that the indices match between lists.
	* Therefore we may do all of our work using the evenVertices list instead of vlist.
	* We then can carefully construct the four triangles.
	*/ 
	for (int i = 0; i < p->tlist.size(); ++i)
	{
		// Start with a triangle:
		Triangle* t = &p->tlist[i];

		//std::cout << "Creating vertices!" << std::endl;
		// Even vertices:
		Vert* v0 = &loop.vlist[t->vertices[0]->index];
		Vert* v1 = &loop.vlist[t->vertices[1]->index];
		Vert* v2 = &loop.vlist[t->vertices[2]->index];

		// Odd vertices:
		int w0Index = oddVertices[t->edges[0]->index]->index;
		int w1Index = oddVertices[t->edges[1]->index]->index;
		int w2Index = oddVertices[t->edges[2]->index]->index;
		Vert* w0 = &loop.vlist[w0Index];
		Vert* w1 = &loop.vlist[w1Index];
		Vert* w2 = &loop.vlist[w2Index];

		int tIndex = loop.tlist.size();

		//std::cout << "Creating triangles: " << std::endl;
		//std::cout << "Decomposing triangle: " << t->index << ": " << t->verts[0]->index << " " << t->verts[1]->index << " " << t->verts[2]->index << std::endl;

		Triangle t1;
		t1.index = tIndex;
		t1.vertices[0] = v0;
		t1.vertices[1] = w0;
		t1.vertices[2] = w2;
		//std::cout << "First triangle: " << v0->index << " " << w0->index << " " << w2->index << std::endl;

		Triangle t2;
		t2.index = tIndex + 1;
		t2.vertices[0] = w0;
		t2.vertices[1] = v1;
		t2.vertices[2] = w1;
		//std::cout << "Second triangle: " << w0->index << " " << v1->index << " " << w1->index << std::endl;

		Triangle t3;
		t3.index = tIndex + 2;
		t3.vertices[0] = w1;
		t3.vertices[1] = v2;
		t3.vertices[2] = w2;
		//std::cout << "Third triangle: " << w1->index << " " << v2->index << " " << w2->index << std::endl;

		Triangle t4;
		t4.index = tIndex + 3;
		t4.vertices[0] = w0;
		t4.vertices[1] = w1;
		t4.vertices[2] = w2;
		//std::cout << "Fourth triangle: " << w0->index << " " << w1->index << " " << w2->index << std::endl;
		//std::cout << std::endl;

		loop.tlist.push_back(t1);
		loop.tlist.push_back(t2);
		loop.tlist.push_back(t3);
		loop.tlist.push_back(t4);
	}

	// Don't forget to initialize the clist!
	Corner c;
	loop.clist = std::vector<Corner>(3 * loop.tlist.size(), c);
	return loop;
}





glm::dvec3 Subdivision::GetAdjacentLinearCombination(Edge* e)
{
	const double LOOP_WEIGHT = (double)3.0 / 8.0;
	Vert* v = e->vertices[0];
	Vert* w = e->vertices[1];

	glm::dvec3 vPosition = LOOP_WEIGHT * v->GetPosition();
	glm::dvec3 wPosition = LOOP_WEIGHT * w->GetPosition();
	return vPosition + wPosition;
}

glm::dvec3 Subdivision::GetOppositeLinearCombination(Edge* e)
{
	if (e->isBoundary())
	{
		std::cout << "ERROR: GetOppositeLinearCombination(Edge* e) should only be called for non-boundary edges!" << std::endl;
		exit(-1);
	}

	const double LOOP_WEIGHT = (double)1.0 / 8.0;
	Triangle* t1 = e->triangles[0];
	Triangle* t2 = e->triangles[1];

	glm::dvec3 v1Position;
	glm::dvec3 v2Position;

	// Find the correct vertices that are opposite to the given edge.
	for (int i = 0; i < 3; ++i)
	{
		if (e->Contains(t1->vertices[i]) == -1)
		{
			Vert* v1 = t1->vertices[i];
			v1Position = LOOP_WEIGHT * v1->GetPosition();
		}
		if (e->Contains(t2->vertices[i]) == -1)
		{
			Vert* v2 = t2->vertices[i];
			v2Position = LOOP_WEIGHT * v2->GetPosition();
		}
	}

	return v1Position + v2Position;
}

glm::dvec3 Subdivision::GetBoundaryLinearCombination(Edge* e)
{
	if (e->isBoundary())
	{
		std::cout << "ERROR: GetBoundaryLinearCombination(Edge* e) should only be called for boundary edges!" << std::endl;
		exit(-1);
	}
	const double LOOP_WEIGHT = (double)1.0 / 2.0;
	glm::dvec3 v1Position = LOOP_WEIGHT * e->vertices[0]->GetPosition();
	glm::dvec3 v2Position = LOOP_WEIGHT * e->vertices[1]->GetPosition();
	return v1Position + v2Position;
}

Vert Subdivision::CreateOddVertex(Edge* e, int index)
{
	glm::dvec3 position;

	// Check if the edge is a boundary or not.
	if (e->isBoundary())
	{
		position = GetBoundaryLinearCombination(e);
	}
	else
	{
		glm::dvec3 adjacent = GetAdjacentLinearCombination(e);
		glm::dvec3 opposite = GetOppositeLinearCombination(e);
		position = adjacent + opposite;
	}
	Vert v(position.x, position.y, position.z);
	v.index = index;
	return v;
}

double Subdivision::Beta(int valence)
{
	if (valence <= 2)
	{
		std::cout << "ERROR: Only call Beta(int valence) for non-boundary vertices. " << std::endl;
		exit(-1);
	}
	else if (valence == 3)
	{
		return (double)3.0 / 16.0;
	}
	else
	{
		return (double)3.0 / (8.0 * (double)valence);
	}
}

std::map<int, Vert*> Subdivision::GetConnectedVertices(Vert* v)
{
	std::map<int, Vert*> connectedVertices;
	// Trace all of the triangles of the vertex v to find the other vertices.
	for (int i = 0; i < v->GetNumberOfTriangles(); ++i)
	{
		Triangle* t = v->triangles[i];
		for (int j = 0; j < 3; ++j)
		{
			Vert* w = t->vertices[j];
			connectedVertices.insert({ w->index, w});
		}
	}
	return connectedVertices;
}

glm::dvec3 Subdivision::GetAdjacentLinearCombination(Vert* v)
{
	std::map<int, Vert*> connectedVertices = GetConnectedVertices(v);
	int n = v->valence;

	// Loop formula:
	glm::dvec3 vPosition = v->GetPosition();
	double scale = 1 - (n * Beta(n));
	glm::dvec3 newPosition = scale * vPosition;

	for (auto it = connectedVertices.begin(); it != connectedVertices.end(); ++it)
	{
		// Make sure not to count the original vertex itself!
		if (it->first != v->index)
		{
			Vert* w = it->second;
			glm::dvec3 wPosition = w->GetPosition();
			newPosition += Beta(n) * wPosition;
		}
	}
	return newPosition;
}

glm::dvec3 Subdivision::GetBoundaryLinearCombination(Vert* v)
{
	std::map<int, Vert*> connectedVertices = GetConnectedVertices(v);
	int n = v->valence;

	// Loop formula:
	glm::dvec3 vPosition = v->GetPosition();
	double scale = (double)3.0 / 4.0;
	glm::dvec3 newPosition = scale * vPosition;

	for (auto it = connectedVertices.begin(); it != connectedVertices.end(); ++it)
	{
		// Make sure not to count the original vertex itself!
		if (it->first != v->index)
		{
			Vert* w = it->second;
			glm::dvec3 wPosition = w->GetPosition();
			newPosition += (double)1.0 / 8.0 * wPosition;
		}
	}
	return newPosition;
}

Vert Subdivision::CreateEvenVertex(Vert* v)
{
	glm::dvec3 newPosition;
	if (v->valence < 3)
	{
		newPosition = GetBoundaryLinearCombination(v);
	}
	else
	{
		newPosition = GetAdjacentLinearCombination(v);
	}
	Vert w;
	w.index = v->index;
	w.x = newPosition.x;
	w.y = newPosition.y;
	w.z = newPosition.z;
	return w;
}











Subdivision::Subdivision() {}
Subdivision::~Subdivision() {}
