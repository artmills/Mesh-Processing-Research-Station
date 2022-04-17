#include "polyhedron.hpp"


Polyhedron::Polyhedron()
{
	vlist.reserve(40000);
	elist.reserve(100000);
	tlist.reserve(40000);
	
	Corner c;
	clist = std::vector<Corner>(3 * tlist.size(), c);
	center = glm::dvec3(0.0, 0.0, 0.0);
}
Polyhedron::Polyhedron(int vertices, int edges, int triangles)
{
	vlist.reserve(vertices);
	elist.reserve(edges);
	tlist.reserve(triangles);
	
	Corner c;
	clist = std::vector<Corner>(3 * tlist.size(), c);
	center = glm::dvec3(0.0, 0.0, 0.0);
}

// Load from a .obj file.
Polyhedron::Polyhedron(std::string file, int a)
{
	vlist.reserve(40000);
	elist.reserve(100000);
	tlist.reserve(40000);

	std::ifstream f(file);

	// Check to see if the file can be opened.
	if (!f)
	{
		std::cout << "FILE COULD NOT BE OPENED." << std::endl;
		exit(-1);
	}

	// Store each line of the file in a string.
	std::string line;

	// Check to see if some line is "# OBJ File".
	/*
	while (f && line != "# OBJ File")
	{
		std::getline(f, line);
	}
	if (!f)
	{
		std::cout << "NOT A .OBJ FILE." << std::endl;
		exit(-1);
	}
	*/

	// Get data from the file.
	std::getline(f, line);
	while (f)
	{
		// Vertex:
		if (line[0] == 'v' && line[1] != 't')
		{
			// Remove the 'v' at the start of the line.
			line.erase(0, line.find(" ") + 1);

			Vert v;
			glm::vec3 position;
			for (int i = 0; i < 3; ++i)
			{
				std::string word = line.substr(0, line.find(" "));
				position[i] = std::stof(word);
				line.erase(0, line.find(" ") + 1);
			}
			v.x = position.x;
			v.y = position.y;
			v.z = position.z;
			v.index = vlist.size();
			vlist.push_back(v);
		}

		// Triangle:
		if (line[0] == 'f')
		{
			// Remove the 't' at the start of the line.
			line.erase(0, line.find(" ") + 1);
			Triangle t;

			// May be formatted as xxx/xxx xxx/xxx xxx/xxx. 
			// Take only the first element of each /.
			for (int i = 0; i < 3; ++i)
			{
				std::string word = line.substr(0, line.find("/"));
				//std::cout << std::stoi(word) << std::endl;
				t.vertices[i] = &vlist[std::stoi(word) - 1];
				line.erase(0, line.find(" ") + 1);
			}
			t.index = tlist.size();
			tlist.push_back(t);
		}

		std::getline(f, line);
	}

	std::cout << vlist.size() << std::endl;
	std::cout << tlist.size() << std::endl;

	Corner c;
	clist = std::vector<Corner>(3 * tlist.size(), c);
	center = glm::dvec3(0.0, 0.0, 0.0);
}
Polyhedron::Polyhedron(std::string file)
{
	vlist.reserve(40000);
	elist.reserve(100000);
	tlist.reserve(40000);

	std::ifstream f(file);

	// Check to see if the file can be opened.
	if (!f)
	{
		std::cout << "FILE COULD NOT BE OPENED." << std::endl;
		exit(-1);
	}

	// Store each line of the file in a string.
	std::string line;

	// Check to see if the first line of the file is "ply".
	std::getline(f, line);
	if (line.substr(0, 3) != "ply")
	{
		std::cout << "NOT A .PLY FILE. " << std::endl;
		exit(-1);
	}

	// Go through the header to figure out how many vertices and triangles there are.
	int numberOfVertices;
	int numberOfTriangles;
	while (f && line.substr(0, 10) != "end_header")
	{
		std::getline(f, line);
		
		// Look for the word "element":
		std::string word = line.substr(0, line.find(" "));
		if (word == "element")
		{
			line.erase(0, line.find(" ") + 1);
			
			// Now decide whether or not this is vertex or triangle info.
			word = line.substr(0, line.find(" "));
			if (word == "vertex")
			{
				line.erase(0, line.find(" ") + 1);
				numberOfVertices = std::stoi(line);
			}
			else if (word == "face")
			{
				line.erase(0, line.find(" ") + 1);
				numberOfTriangles = std::stoi(line);
			}
			else
			{
				std::cout << "ERROR READING VERTEX/FACE INFO." << std::endl;
				exit(-1);
			}
		}
	}

	// Now that we know how many vertices/faces there are, we can build up the lists.
	// The file is currently sitting at the line "end_header". 
	int count = 0;
	std::getline(f, line);
	while (f && line != "")
	{
		// Vertex:
		if (count < numberOfVertices)
		{
			Vert v;
			glm::vec3 position;
			for (int i = 0; i < 3; ++i)
			{
				std::string word = line.substr(0, line.find(" "));
				position[i] = std::stof(word);
				line.erase(0, line.find(" ") + 1);
			}
			v.x = position.x;
			v.y = position.y;
			v.z = position.z;
			v.index = vlist.size();
			vlist.push_back(v);
		}
		// Triangle:
		else
		{
			// Check that the first word is 3:
			std::string word = line.substr(0, line.find(" "));
			if (std::stoi(word) != 3)
			{
				std::cout << "ERROR: NOT A TRIANGLE MESH." << std::endl;
			}
			line.erase(0, line.find(" ") + 1);
		
			// Continue parsing:
			Triangle t;	
			for (int i = 0; i < 3; ++i)
			{
				std::string word = line.substr(0, line.find(" "));
				t.vertices[i] = &vlist[std::stoi(word)];
				line.erase(0, line.find(" ") + 1);
			}
			t.index = tlist.size();
			tlist.push_back(t);
		}
		++count;
		std::getline(f, line);
	}

	Corner c;
	clist = std::vector<Corner>(3 * tlist.size(), c);
	center = glm::dvec3(0.0, 0.0, 0.0);
}

Polyhedron::~Polyhedron() {}
/*
Polyhedron::Polyhedron(const Polyhedron& p)
{
	this->index = p.index;
	this->surfaceArea = p.surfaceArea;
	this->center = p.center;
	this->radius = p.radius;
	this->valenceDeficit = p.valenceDeficit;
	this->angleDeficit = p.angleDeficit;
	this->vlist = p.vlist;
	this->elist = p.elist;
	this->tlist = p.tlist;
	this->clist = p.clist;
}
Polyhedron::Polyhedron(Polyhedron&& p)
{
	this->index = p.index;
	this->surfaceArea = p.surfaceArea;
	this->center = p.center;
	this->radius = p.radius;
	this->valenceDeficit = p.valenceDeficit;
	this->angleDeficit = p.angleDeficit;
	this->vlist = p.vlist;
	this->elist = p.elist;
	this->tlist = p.tlist;
	this->clist = p.clist;
}
Polyhedron& Polyhedron::operator=(Polyhedron&& p)
{
	this->index = p.index;
	this->surfaceArea = p.surfaceArea;
	this->center = p.center;
	this->radius = p.radius;
	this->valenceDeficit = p.valenceDeficit;
	this->angleDeficit = p.angleDeficit;
	this->vlist = p.vlist;
	this->elist = p.elist;
	this->tlist = p.tlist;
	this->clist = p.clist;
	return *this;
}
*/


void Polyhedron::Initialize()
{
	std::cout << std::endl;
	std::cout << "***** Initializing Polyhedron *****" << std::endl;
	std::cout << "***** Connecting vertices to triangles ***** " << std::endl;
	ConnectVerticesToTriangles();

	std::cout << "***** Creating edges ***** " << std::endl;
	CreateEdges();
	std::cout << "Polyhedron has " << vlist.size() << " vertices, " << elist.size() << " edges, and " << tlist.size() << " triangles. " << std::endl;

	std::cout << "***** Ordering pointers ***** " << std::endl;
	for (int i = 0; i < vlist.size(); ++i)
	{
		OrderVertexToTrianglePointers(vlist[i]);
	}

	std::cout << "***** Computing edge lengths ***** " << std::endl;
	for (int i = 0; i < elist.size(); ++i)
	{
		elist[i].ComputeLength();
	}

	std::cout << "***** Computing bounding sphere ***** " << std::endl;
	ComputeBoundingSphere();

	std::cout << "***** Computing normals and areas ***** " << std::endl;
	ComputeNormalsAndArea();

	std::cout << "***** Interpolating normals ***** " << std::endl;
	InterpolateNormals();

	std::cout << "***** Constructing corner list *****" << std::endl;
	MeshAnalysis::GetCornerList(this);

	std::cout << "***** Computing valence deficit *****" << std::endl;
	MeshAnalysis::GetValenceDeficit(this);
	std::cout << "Valence deficit is " << valenceDeficit << std::endl;

	std::cout << "***** Computing angle deficit *****" << std::endl;
	MeshAnalysis::GetAngleDeficit(this);
	std::cout << "Angle deficit is " << angleDeficit << std::endl;

	std::cout << std::endl;
}




void Polyhedron::ConnectVerticesToTriangles()
{
	// Go through the triangles:
	for (int i = 0; i < tlist.size(); ++i)
	{
		Triangle* t = &tlist[i];	

		// Add a pointer to the current triangle to each of its vertices.
		for (int j = 0; j < 3; ++j)
		{
			Vert* v = t->vertices[j];
			v->triangles.push_back(t);
			v->numberOfTriangles++;
		}
	}
}

void Polyhedron::CreateEdge(Vert* v0, Vert* v1)
{
	//std::cout << "***** Attempting to create edge between vertices " << v0->index << " and " << v1->index << " *****" << std::endl;
	// First create an edge.
	// Establish all properties of this edge before we push it to the 
	// edge list.
	Edge edge;
	edge.index = elist.size();
	edge.vertices[0] = v0;
	edge.vertices[1] = v1;
	elist.push_back(edge);
	//std::cout << "Pushing new edge #" << edge.index << " to elist." << std::endl;
	Edge* e = &elist[edge.index];

	// Go through the triangles of the first vertex v0.
	// If any of these triangles contains the second vertex v1:
	// add this triangle to the edge's list of triangles.
	for (int i = 0; i < v0->numberOfTriangles; ++i)
	{
		Triangle* t = v0->triangles[i];

		int k = t->Contains(v1);
		if (k != -1)
		{
			// The triangle contains both v0 and v1.
			// Establish this edge as an edge for the triangle list.
			// For consistency's sake, the edge will be always be added as the (j+1)%3 index'ed edge of the triangle, where j is the index of the vertex of the triangle.
			e->triangles.push_back(t);
			e->numberOfTriangles++;

			int index0 = t->Contains(v0);
			if ((k + 1) % 3 == index0)
			{
				t->edges[k] = &elist[edge.index];
				//std::cout << "Successfully registered edge #" << e->index << " to triangle #" << t->index << " at location " << k << std::endl;
			}
			else if ((k + 2) % 3 == index0)
			{
				t->edges[(k+2)%3] = &elist[edge.index];
				//std::cout << "Successfully registered edge #" << e->index << " to triangle #" << t->index << " at location " << (k+2)%3 << std::endl;
			}
			else
			{
				std::cout << "TRIANGULATION ERROR AT TRIANGLE " << t->index << std::endl;
			}
		}
	}
}


void Polyhedron::CreateEdges()
{
	// Loop through the triangles of the polyhedron.
	// Create edges between vertices using the CreateEdge() method.
	// Skip over triangle edges that have already been created.
	// To ensure that this works, first set all edge pointers to NULL.
	for (int i = 0; i < tlist.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tlist[i].edges[j] = NULL;
		}
	}

	// Now get to creating edges.
	for (int i = 0; i < tlist.size(); ++i)
	{
		//std::cout << "BEGIN ANALYSIS OF TRIANGLE " << i << std::endl;
		Triangle t = tlist[i];
		for (int j = 0; j < 3; ++j)
		{
			// If this edge exists, skip.
			if (!t.edges[j])
			{
				Vert* v0 = t.vertices[j];
				Vert* v1 = t.vertices[(j+1)%3];
				CreateEdge(v0, v1);
			}
		}
	}
}


void Polyhedron::OrderVertexToTrianglePointers(Vert v)
{
	// Start at the 0th triangle.
	Triangle* t = v.triangles[0];
	Triangle* next;

	// Go backwards (clockwise) around triangles around a vertex to find the boundary.
	int k = -1;
	for (int i = 1; i <= v.numberOfTriangles; ++i)
	{
		// First, get the index of v in t.
		k = t->Contains(&v);
		if (k == -1)
		{
			break;
		}

		// The corresponding face is the previous one around v.
		Edge* e = t->edges[k];	

		next = e->GetOtherTriangle(t);

		// If this is a boundary:
		if (next == NULL)
		{
			// Find a reference to t in v.
			for (int j = 0; j < v.numberOfTriangles; ++j)
			{
				if (v.triangles[j] == t)
				{
					v.triangles[j] = v.triangles[0];
					v.triangles[0] = t;
					break;
				}
			}
		}
		else
		{
			t = next;
		}
	}

	// Now walk around in the forward direction and place them in order.
	t = v.triangles[0];
	int count = 0;
	
	for (int i = 0; i < v.numberOfTriangles; ++i)
	{
		// Find reference to v in t.
		k = t->Contains(&v);
		if (k == -1)
		{
			break;
		}
		 // The corresponding face is the next one around v.
		 Edge* e = t->edges[k];
		 next = e->GetOtherTriangle(t);

		 // Break if we reached a boundary;
		 if (next == NULL)
		 {
			 break;
		 }

		 // Swap the next face into its proper place in the face list.
		 for (int j = 0; j < v.numberOfTriangles; ++j)
		 {
			if (v.triangles[j] == next)
			{
				v.triangles[j] = v.triangles[i];
				v.triangles[i] = next;
				break;
			}
		 }
		 t = next;
	}
	
}

void Polyhedron::ComputeBoundingSphere()
{
	glm::dvec3 min = glm::dvec3(vlist[0].x, vlist[0].y, vlist[0].z);
	glm::dvec3 max = min;
	for (int i = 0; i < vlist.size(); ++i)
	{
		if (vlist[i].x < min.x)
			min.x = vlist[i].x;
		if (vlist[i].x > max.x)
			max.x = vlist[i].x;
		if (vlist[i].y < min.y)
			min.y = vlist[i].y;
		if (vlist[i].y > max.y)
			max.y = vlist[i].y;
		if (vlist[i].z < min.z)
			min.z = vlist[i].z;
		if (vlist[i].z > max.z)
			max.z = vlist[i].z;
	}
	center = 0.5 * (min + max);
	radius = glm::length(center - min);
}

void Polyhedron::ComputeNormalsAndArea()
{
	double signedVolume = 0.0;

	// Go through the triangle list and call the method on each triangle.
	for (int i = 0; i < tlist.size(); ++i)
	{
		Triangle& t = tlist[i];

		// Compute the normal and area of this triangle.
		t.ComputeNormalAndArea();

		// Add to the total surface area.
		surfaceArea += t.area;

		glm::dvec3 first(t.vertices[0]->x, t.vertices[0]->y, t.vertices[0]->z);
		signedVolume += glm::dot(center - first, t.normal) * t.area;
	}

	// Now orient the normals in the triangles:
	if (signedVolume > 0)
	{
		for (int i = 0; i < tlist.size(); ++i)
		{
			tlist[i].normal *= -1.0;
		}
	}
}

void Polyhedron::InterpolateNormals()
{
	for (int i = 0; i < vlist.size(); ++i)
	{
		Vert& v = vlist[i];
		for (int j = 0; j < v.numberOfTriangles; ++j)
		{
			v.normal += v.triangles[j]->normal;
		}
		v.normal = glm::normalize(v.normal);
	}
}

void Polyhedron::PrintVertices()
{
	for (int i = 0; i < vlist.size(); ++i)
	{
		vlist[i].Print();
	}
}
void Polyhedron::PrintEdges()
{
	for (int i = 0; i < elist.size(); ++i)
	{
		elist[i].Print();
	}
}
void Polyhedron::PrintTriangles()
{
	for (int i = 0; i < tlist.size(); ++i)
	{
		tlist[i].Print();
	}
}
void Polyhedron::PrintCorners()
{
	for (int i = 0; i < clist.size(); ++i)
	{
		clist[i].Print();
	}
}
