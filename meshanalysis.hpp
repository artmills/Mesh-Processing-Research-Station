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

	// Compute perimeter or area of a single triangle.
	static float ComputePerimeter(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputePerimeter(Triangle& t);
	static float ComputeArea(Vertex& v0, Vertex& v1, Vertex& v2);
	static double ComputeAreaDouble(Triangle& t);
	static double ComputePerimeterDouble(Triangle& t);
	static double ComputeSphericalAreaDouble(glm::dvec3& n0, glm::dvec3& n1, glm::dvec3& n2);
	static float ComputeSphericalArea(glm::vec3& n0, glm::vec3& n1, glm::vec3& n2);

	// Compute area of the horizon viewer set of a single triangle.
	static float ComputeHorizonArea(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputeHorizonArea(Triangle& t);
	static double ComputeHorizonAreaDouble(Triangle& t);

	// Compute horizon measure.
	static float ComputeHorizonMeasure(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputeOriginalHorizonMeasure(Vertex& v0, Vertex& v1, Vertex& v2);
	static float ComputeHorizonMeasure(Triangle& t);
	static double ComputeHorizonMeasureDouble(Triangle& t);
	static double ComputeOriginalHorizonMeasureDouble(Triangle& t);

	// Compute approximate Gaussian curvature.
	static double ComputeApproximateGaussianCurvature(Triangle& t);
	static float ComputeApproximateGaussianCurvature(Vertex& v0, Vertex& v1, Vertex& v2);

	// Get the horizon measure of every triangle.
	static std::vector<float> GetHorizonMeasures(std::vector<Triangle>& triangles);
	static std::vector<double> GetHorizonMeasuresDouble(std::vector<Triangle>& triangles);
	static std::vector<double> GetOriginalHorizonMeasuresDouble(std::vector<Triangle>& triangles);

	// Get the approximate Gaussian curvature of every triangle.
	static std::vector<double> GetApproximateGaussianCurvatures(std::vector<Triangle>& triangles);

private:

	MeshAnalysis();
	~MeshAnalysis();

	float static InverseLerp(float start, float end, float v);
};
