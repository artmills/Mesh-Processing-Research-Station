#pragma once

#include <cmath>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#include "glm/gtx/norm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "polyhedron.hpp"
#include "tangentspace.hpp"
#include "smoothing.hpp"

/** Implementation of curvature operators from:
 * http://www.multires.caltech.edu/pubs/diffGeoOps.pdf
 * from Project 3.
 * This includes algorithms for:
 * 1) Mean curvature H.
 * 2) Gaussian curvature K.
 * 3) Curvature tensor.
 */
class Curvature
{

public:

	// Given a corner, compute the area of the Voronoi region containing the vertex corresponding to the corner.
	static double ComputeMixedArea(Corner& c);

	// Compute the mean curvature at the vertex.
	static glm::dvec3 ComputeMeanCurvature(Corner& c);

	// Compute the Gaussian curvature at the vertex.
	static double ComputeGaussianCurvature(Corner& c);

	// Compute the normal curvature in the direction of an edge.
	static double ComputeNormalCurvature(Vert* v1, Vert* v2);

	// Get the curvature tensor and principal curvatures by solving a linear system.
	static TangentSpace GetCurvatureTensor(Corner& c);

	// Apply smoothing to the curvature tensors.
	static void SmoothCurvatureTensor(Polyhedron* p, std::vector<TangentSpace>& tangentSpaces, double dt, int iterations);

	// Get eigen-stuff:
	static void GetEigenSpaces(std::vector<TangentSpace>& tangentSpaces);

	// Set streamlines:
	static void GetTriangleStreamlines(Polyhedron* p, std::vector<TangentSpace>& tangentSpaces, std::vector<Eigen::Vector3d>& minorStream, std::vector<Eigen::Vector3d>& majorStream);

private:

	static double MeanCurvatureWeight(double theta, double phi);

	Curvature();
	~Curvature();

};
