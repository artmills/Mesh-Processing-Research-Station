#pragma once

#include <cmath>
#include <limits>
#include <algorithm>
#include "utilities.hpp"
#include "meshcomponent.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/norm.hpp"

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

	// Get the star of a vertex, using the corner list to ensure orientation.
	std::vector<Triangle*> static GetVertexStar(Vert* v);

	
	/********* LINES OF CURVATURE *********/
	// Get principal directions:
	// Data is structured such that two elements in a row are the max and min principal directions at the same vertex.
	static std::vector<Edge*> GetPrincipalDirections(Polyhedron *p);



	/********* VERTEX CURVATURES *********/

	static std::vector<double> GetVertexCurvatures(Polyhedron* p, Curvature curvature);
	static double GetVertexCurvature(Vert* v, Curvature curvature);


	/********* CURVATURE *********/

	// Given a corner, compute the area of the Voronoi region containing the vertex corresponding to the corner.
	static double ComputeMixedArea(Corner& c);

	// Given a corner, compute the perimeter of the Voronoi region containing the vertex corresponding to the corner.
	static double ComputeMixedPerimeter(Corner& c);

	// Compute the angle between the max/min principal distortions.
	static double ComputePrincipalDeviation(Corner& c);

	// Compute the Gaussian curvature at the vertex.
	static double ComputeGaussianCurvature(Corner& c);

	// Compute the mean curvature at the vertex.
	static double ComputeMeanCurvature(Corner& c);

	// Compute the signed mean curvature at the vertex.
	static double ComputeSignedMeanCurvature(Corner& c);

	// Compute the distortion at the vertex.
	static double ComputeSignedDistortion(Corner& c);

	// Compute the distortion at the vertex.
	static double ComputeDistortion(Corner& c);

	// Compute the principal curvatures from distortion.
	static double ComputeMaxPrincipalDistortion(Corner& c);
	static double ComputeMinPrincipalDistortion(Corner& c);

	// Compute the ``Gaussian/mean curvature'' from distortion.
	static double ComputeGaussianFromDistortion(Corner& c);
	static double ComputeMeanFromDistortion(Corner& c);
	
	// Compute the Gaussian curvature of the dual cone at the vertex.
	static double ComputeGaussianCone(Corner& c);

	// Compute the length of the boundary of the Gauss map.
	static double ComputeHorizonMeasure(Corner& c);
	static double ComputeHorizonMeasureTest(Corner& c);

	static Edge* GetEdge(Triangle* t0, Triangle* t1);

private:

	MeshAnalysis();
	~MeshAnalysis();

	float static InverseLerp(float start, float end, float v);
	static double MeanCurvatureWeight(double theta, double phi);

};
