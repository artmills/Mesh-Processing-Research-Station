#pragma once

#include <map>
#include <cmath>
#include "polyhedron.hpp"
#include "glm/glm.hpp"

/** Smooth a mesh according to four different weighting schemes:
 * 1) Uniform.
 * 2) Cord.
 * 3) Mean curvature flow.
 * 4) Mean value coordinates.
 */
enum class Weight
{
	UNIFORM,
	CORD_DYNAMIC,
	CORD_STATIC,
	MEAN_CURVATURE_DYNAMIC,
	MEAN_CURVATURE_STATIC,
	MEAN_VALUE_DYNAMIC,
	MEAN_VALUE_STATIC
};
class Smoothing
{

public:

	// Smoothing algorithms:
	void static SmoothMesh(Polyhedron* p, double dt, Weight weight);

	// Morse design:
	// Given a corner, find the correct color for its vertex.
	void static EvaluateMorse0(Polyhedron* p, std::vector<int>& maxima, std::vector<int>& minima, double defaultValue, double dt, int iterations);
	void static EvaluateMorse1(Polyhedron* p, std::vector<int>& maxima, std::vector<int>& minima, double defaultValue, double dt, int iterations);

	// Critical point analysis:
	// Vert.minMax = 0: local minimum.
	// Vert.minMax = 1: local maximum.
	// Vert.saddle = k: k-saddle.
	void static SetCriticalPoints0(Polyhedron* p);	
	void static SetCriticalPoints1(Polyhedron* p);	

private:

	/*
	bool static isLocalMaximum0(Vert* v, std::vector<Vert*> neighbors);
	bool static isLocalMaximum0(Vert* v, std::vector<Vert*> neighbors);
	bool static isLocalMinimum1(Vert* v, std::vector<Vert*> neighbors);
	bool static isLocalMinimum1(Vert* v, std::vector<Vert*> neighbors);
	*/

	double static UniformWeight(int total);
	double static CordWeight(Vert* v, Vert* w);
	double static MeanCurvatureWeight(double theta, double phi);
	double static MeanValueWeight(double theta, double phi);
	bool static isIn(int target, std::vector<int> list);

	Smoothing();
	~Smoothing();

};
