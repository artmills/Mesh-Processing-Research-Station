#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

/** This file contains the three pieces of geometry that builds up a mesh:
 * 1) Vert
 * 2) Edge
 * 3) Triangle.
 * These classes will hold data needed to construct a mesh.
 * 
 * The goal is to be able to convert a .ply file into a mesh that can be viewed and analyzed in code.
 * A .ply file provides the information needed to construct vertices and organize them into triangles.
 * It is up to us to reconstruct adjacency information using this data.
 * To do this, vertex, edges, and triangles will contain arrays of other such objects to keep track of which objects are adjacent to which other ones.
 */

// Forward declarations:
class Edge;
class Triangle;
class Corner;


/** Vert **/
class Vert
{

public:

	Vert();
	Vert(double x, double y, double z);
	~Vert();
	Vert(const Vert& v);
	Vert(Vert&& v);
	Vert& operator=(Vert&& v);

	// Info dump this vertex:
	void Print();

	// ID:
	int index = -1;

	// 3D Positions:
	double x, y, z;
	
	// 3D Normal vector:
	glm::dvec3 normal;

	// Number of triangles attached to this vertex:
	int numberOfTriangles = 0;

	// Valence:
	int valence = 0;

	// Total angle:
	double totalAngle = 0.0;

	// For Morse design: a function value.
	double value0 = 0.0;
	double value1 = 0.0;

	// For Morse analysis: critical point information.
	int minMax = -1;
	int saddle = -1;

	// List of triangles attached to this vertex:
	std::vector<Triangle*> triangles;
};


/** Edge **/
class Edge
{

public:

	Edge();
	~Edge();
	Edge(const Edge& e);
	Edge(Edge&& e);
	Edge& operator=(Edge&& e);

	void Print();

	bool isBoundary();
	Triangle* GetOtherTriangle(Triangle* t);
	Vert* GetOtherVertex(Vert* v);
	void ComputeLength();
	int Contains(Vert* v);


	// ID:
	int index = -1;

	// Length:
	double length = -1.0;

	// List of vertices attached to this edge:
	std::vector<Vert*> vertices;

	// Number of triangles attached to this edge:
	int numberOfTriangles = 0;

	// List of triangles attached to this edge:
	std::vector<Triangle*> triangles;
};


/** Triangle **/
class Triangle
{

public:

	Triangle();
	~Triangle();
	Triangle(const Triangle& t);
	Triangle(Triangle&& t);
	Triangle& operator=(Triangle&& t);

	// Info dump this triangle:
	void Print();

	int Contains(Vert* v);
	void ComputeNormalAndArea();

	// ID:
	int index = -1;

	// Normal:
	glm::dvec3 normal;

	// Area:
	double area = -1.0;

	// List of vertices attached to this triangle:
	std::vector<Vert*> vertices;

	// List of edges attached to this triangle:
	std::vector<Edge*> edges;
};


/** A corner is an object representing a vertex in the interior of a triangle.
 * Each triangle has three corners -- one at each vertex.
 * Two corners are equivalent if they correspond to the same vertex and are located within the same triangle.
 */
class Corner
{

public:

	Corner();
	~Corner();
	Corner(const Corner& v);
	Corner(Corner&& v);
	Corner& operator=(Corner&& v);

	// Get all adjacent vertices to c.v.
	std::vector<Vert*> GetAdjacentVertices();

	void Print();

	// ID:
	int index = -1;

	// Corresponding geometry:
	Vert* v = NULL; // Vertex attached to this corner.
	Edge* e = NULL; // Edge opposite the corner.
	Triangle* t = NULL; // Triangle containing this corner.

	Corner* n = NULL; // The next corner, following orientation.
	Corner* p = NULL; // The previous corner, folowing orientation.
	Corner* o = NULL; // The corner in a different triangle that has the same opposite edge.

	double angle = 0.0;

};
