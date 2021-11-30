#pragma once

#include <cmath>
#include <limits>
#include <algorithm>
#include "meshcomponent.hpp"

// Forward declaration.
class Polyhedron;
class MeshComponent;

// Static class to analyze a Polyhedron object.
class MeshAnalysis
{

public:

	// Compute the corners of a polyhedron.
	void static GetCornerList(Polyhedron* p);	

	// Compute the valence deficit of a polyhedron.
	void static GetValenceDeficit(Polyhedron* p);

	// Compute the angle at a single vertex.
	void static ComputeAngle(Corner& c);

	// Compute all angles of a list of corners. 
	void static ComputeAngles(std::vector<Corner>& corners);

	// Compute angle deficit of a polyhedron.
	void static GetAngleDeficit(Polyhedron* p);

	// Compute relative horizon viewer set area.
	double static GetHorizonMeasure(Triangle t);
	
	// Compute area of the horizon viewer set.
	double static GetHorizonArea(Triangle t);
	float static GetHorizonArea(Vertex v0, Vertex v1, Vertex v2);

	// Get the horizon measure for each triangle, H_V / V.
	void static HorizonMeasureAnalysis(Polyhedron* p);

	// Get alternative horizon measure: Area(H_V) / Length(V).
	void static HorizonMeasureLength(Polyhedron* p);
	void static HorizonMeasureLength(std::vector<MeshComponent>& meshes);
	
	// Get alternative horizon measure: Area(H_V) / Length(V).
	std::vector<double> static GetHorizonMeasureLength(Polyhedron* p);
	std::vector<double> static GetHorizonMeasureLength2(Polyhedron* p);

	// Get an average horizon measure for each vertex.
	std::vector<double> static ComputeHorizonMeasure(Polyhedron* p);


private:

	MeshAnalysis();
	~MeshAnalysis();

	float static InverseLerp(float start, float end, float v);
};
