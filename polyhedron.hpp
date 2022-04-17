#pragma once

#include <fstream>
#include <string>
#include "geometry.hpp"
#include "meshanalysis.hpp"

// Mesh class with adjacency information through vertices, edges, and triangles.
class Polyhedron
{

public:

	Polyhedron();
	Polyhedron(int vertices, int edges, int triangles);

	// Create a polyhedron from reading in a .ply file.
	Polyhedron(std::string file);
	Polyhedron(std::string file, int a);
	//Polyhedron(std::vector<MeshComponent>& meshes);
	~Polyhedron();
	/*
	Polyhedron(const Polyhedron& p);
	Polyhedron(Polyhedron&& p);
	Polyhedron& operator=(Polyhedron&& p);
	*/


	// Do all of the operations to prepare this mesh.
	void Initialize();

	// Info dump:
	void PrintVertices();
	void PrintEdges();
	void PrintTriangles();
	void PrintCorners();



	// ID:
	int index = -1;

	// Surface area:
	double surfaceArea = 0.0;

	// Bounding sphere:
	glm::dvec3 center;
	double radius = 0.0;

	// Valence and angle deficits:
	int valenceDeficit = 0;
	double angleDeficit = 0.0;

	// Geometry lists:
	std::vector<Vert> vlist;
	std::vector<Edge> elist;
	std::vector<Triangle> tlist;
	std::vector<Corner> clist;

private:

	// Create pointers from vertices to their triangles.
	void ConnectVerticesToTriangles();

	// Create an edge between two vertices and add it to elist.
	void CreateEdge(Vert* v0, Vert* v1);

	// Create all edges, triangle by triangle.
	void CreateEdges();

	// Order the triangles around a vertex counterclockwise.
	void OrderVertexToTrianglePointers(Vert v);

	// Compute the radius and center of a sphere that bounds the mesh.
	void ComputeBoundingSphere();

	// Compute the face normals and area, including orientation.
	void ComputeNormalsAndArea();

	// Interpolate normals to the vertices.
	void InterpolateNormals();

};
