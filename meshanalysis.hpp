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

	// Compute perimeter of a single triangle.
	static float ComputePerimeter(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputePerimeter(Triangle& t);
	static double ComputePerimeterDouble(Triangle& t);

	// Compute area of the horizon viewer set of a single triangle.
	static float ComputeHorizonArea(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputeHorizonArea(Triangle& t);
	static double ComputeHorizonAreaDouble(Triangle& t);

	// Compute horizon measure.
	static float ComputeHorizonMeasure(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputeHorizonMeasure(Triangle& t);
	static double ComputeHorizonMeasureDouble(Triangle& t);

	// Get the horizon measure of every triangle.
	static std::vector<float> GetHorizonMeasures(std::vector<Triangle>& triangles);
	static std::vector<double> GetHorizonMeasuresDouble(std::vector<Triangle>& triangles);

private:

	MeshAnalysis();
	~MeshAnalysis();

	float static InverseLerp(float start, float end, float v);
};
