#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
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

	/*** Constructors ***/
	Vert();
	Vert(double x, double y, double z);
	~Vert();
	Vert(const Vert& v);
	Vert(Vert&& v);
	Vert& operator=(Vert&& v);


	/*** Methods ***/

	// Info dump this vertex:
	void Print();

	// Get number of triangles attached to this vertex.
	int GetNumberOfTriangles();

	// Get position vector.
	glm::dvec3 GetPosition();


	/*** Fields ***/

	int index = -1;
	double x, y, z;
	glm::dvec3 normal;
	double totalAngle = 0.0;

	// Number of edges attached to the vertex.
	int valence = 0;

	// One of the corners of the vertex: the others can be found by corner traversal.
	Corner* c;
	std::vector<Triangle*> triangles;
};


/** Edge **/
class Edge
{

public:

	/*** Constructors ***/
	Edge();
	~Edge();
	Edge(const Edge& e);
	Edge(Edge&& e);
	Edge& operator=(Edge&& e);


	/*** Methods ***/
	void Print();
	double GetLength();

	// Return true if there is only one triangle attached to this edge.
	bool isBoundary();

	// Returns NULL if boundary or given triangle is not attached to this edge.
	Triangle* GetOtherTriangle(Triangle* t);

	// Returns NULL if given vertex is not attached to this edge.
	Vert* GetOtherVertex(Vert* v);

	// Return 0 or 1 if the given vertex is at index 0 or 1 in the vertex list, or return -1 if the vertex is not attached to the edge.
	int Contains(Vert* v);


	/*** Fields ***/

	int index = -1;
	std::vector<Vert*> vertices;
	std::vector<Triangle*> triangles;
};


/** Triangle **/
class Triangle
{

public:

	/*** Constructors ***/
	Triangle();
	~Triangle();
	Triangle(const Triangle& t);
	Triangle(Triangle&& t);
	Triangle& operator=(Triangle&& t);


	/*** Methods ***/
	void Print();
	void ComputeNormalAndArea();

	// Returns the index of the vertex in the list of vertices, or -1 if it is not attached to the triangle.
	int Contains(Vert* v);

	// ID:
	int index = -1;
	glm::dvec3 normal;

	// Area: keep in memory since it does involve a square root, I guess.
	double area = -1.0;

	std::vector<Vert*> vertices;
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
