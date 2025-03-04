#include "meshanalysis.hpp"
#include <limits>


MeshAnalysis::MeshAnalysis() {}
MeshAnalysis::~MeshAnalysis() {}

std::vector<Edge*> MeshAnalysis::GetPrincipalDirections(Polyhedron* p)
{
	// Two directions for each vertex.
	std::vector<Edge*> principals;
	principals.reserve(p->vlist.size() * 2);

	Edge* maxDirection;
	Edge* minDirection;

	// Go through the corner list and compute the principal directions of the corresponding vertex.
	// Skip vertices we have already seen.
	for (Vert& v : p->vlist)
	{
		// Min/max principal curvatures using the distortion.
		double max = -1.0 * std::numeric_limits<double>::max();
		double min = std::numeric_limits<double>::max();

		int index = v.index;

		glm::dvec3 vPosition = v.GetPosition();
		std::vector<Triangle*> star = GetVertexStar(&v);
		star.push_back(star[0]);

		// Remember: star[size - 1] = star[0].
		for (int i = 0; i < star.size() - 1; ++i)
		{
			// Normals:
			glm::dvec3 current = star[i]->normal;
			glm::dvec3 next = star[i+1]->normal;

			// Angle between normals:
			double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
			double angle = acos(dot);

			// Figure out if the angle should be signed:
			// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

			// Get the tangent vector to the edge.
			Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
			if (e == NULL)
				std::cout << "OOPS" << std::endl;
			Vert* q = e->GetOtherVertex(&v);
			glm::dvec3 te = q->GetPosition() - vPosition;
			if (q->index == v.index)
				std::cout << "OOPS" << std::endl;

			// DEBUG: check that the edge is correct.
			Triangle* t1 = e->GetOtherTriangle(star[i]);
			Triangle* t0 = e->GetOtherTriangle(star[i+1]);
			if (t1->index != star[i+1]->index || t0->index != star[i]->index)
				std::cout << "EDGE BROKE" << std::endl;

			// Check the inner product.
			double ip = glm::dot(glm::cross(current, next), te);
			if (ip > 0)
				angle *= -1.0;

			// Check for min or max.
			if (angle > max)
			{
				max = angle;
				maxDirection = e;
			}
			if (angle < min)
			{
				min = angle;
				minDirection = e;
			}
		}
		principals.push_back(maxDirection);
		principals.push_back(minDirection);
	}
	return principals;
}

double MeshAnalysis::ComputePrincipalDeviation(Corner& c)
{
	// Get the vertex and its star.
	// Set the last element to be equal to the first so all the calculations can be done in a single for-loop.
	Vert* v = c.v;
	glm::dvec3 vPosition = v->GetPosition();
	std::vector<Triangle*> star = GetVertexStar(v);
	star.push_back(star[0]);

	//std::cout << "Current vertex is " << v->index << std::endl;
	if (c.o == NULL)
		std::cout << "BOUNDARY" << std::endl;

	// Min/max principal curvatures using the distortion.
	double min = std::numeric_limits<double>::max();
	double max = -1.0 * std::numeric_limits<double>::max();
	glm::dvec3 minDirection;
	glm::dvec3 maxDirection;

	// Remember: star[size - 1] = star[0].
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Figure out if the angle should be signed:
		// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

		// Get the tangent vector to the edge.
		Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
		if (e == NULL)
			std::cout << "OOPS" << std::endl;
		Vert* q = e->GetOtherVertex(v);
		glm::dvec3 te = q->GetPosition() - vPosition;
		if (q->index == v->index)
			std::cout << "OOPS" << std::endl;

		// DEBUG: check that the edge is correct.
		Triangle* t1 = e->GetOtherTriangle(star[i]);
		Triangle* t0 = e->GetOtherTriangle(star[i+1]);
		if (t1->index != star[i+1]->index || t0->index != star[i]->index)
			std::cout << "EDGE BROKE" << std::endl;

		// Check the inner product.
		double ip = glm::dot(glm::cross(current, next), te);
		if (ip > 0)
			angle *= -1.0;

		// Check for min or max.
		if (angle > max)
		{
			max = angle;
			maxDirection = te / glm::length(te);
		}
		if (angle < min)
		{
			min = angle;
			minDirection = te / glm::length(te);
		}
	}
	
	double principalAngle = glm::acos(glm::clamp(glm::dot(maxDirection, minDirection), -1.0, 1.0));
	return 0.5 * M_PI - principalAngle;
}

double MeshAnalysis::ComputeMeanFromDistortion(Corner& c)
{
	// Get the vertex and its star.
	// Set the last element to be equal to the first so all the calculations can be done in a single for-loop.
	Vert* v = c.v;
	glm::dvec3 vPosition = v->GetPosition();
	std::vector<Triangle*> star = GetVertexStar(v);
	star.push_back(star[0]);

	//std::cout << "Current vertex is " << v->index << std::endl;
	if (c.o == NULL)
		std::cout << "BOUNDARY" << std::endl;

	// Min/max principal curvatures using the distortion.
	double min = std::numeric_limits<double>::max();
	double max = -1.0 * std::numeric_limits<double>::max();

	// Remember: star[size - 1] = star[0].
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Figure out if the angle should be signed:
		// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

		// Get the tangent vector to the edge.
		Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
		if (e == NULL)
			std::cout << "OOPS" << std::endl;
		Vert* q = e->GetOtherVertex(v);
		glm::dvec3 te = q->GetPosition() - vPosition;
		if (q->index == v->index)
			std::cout << "OOPS" << std::endl;

		// DEBUG: check that the edge is correct.
		Triangle* t1 = e->GetOtherTriangle(star[i]);
		Triangle* t0 = e->GetOtherTriangle(star[i+1]);
		if (t1->index != star[i+1]->index || t0->index != star[i]->index)
			std::cout << "EDGE BROKE" << std::endl;

		// Check the inner product.
		double ip = glm::dot(glm::cross(current, next), te);
		if (ip > 0)
			angle *= -1.0;

		// Check for min or max.
		if (angle > max)
			max = angle;
		if (angle < min)
			min = angle;
	}
	return 0.5 * (min + max);
}

double MeshAnalysis::ComputeGaussianFromDistortion(Corner& c)
{
	// Get the vertex and its star.
	// Set the last element to be equal to the first so all the calculations can be done in a single for-loop.
	Vert* v = c.v;
	glm::dvec3 vPosition = v->GetPosition();
	std::vector<Triangle*> star = GetVertexStar(v);
	star.push_back(star[0]);

	//std::cout << "Current vertex is " << v->index << std::endl;
	if (c.o == NULL)
		std::cout << "BOUNDARY" << std::endl;

	// Min/max principal curvatures using the distortion.
	double min = std::numeric_limits<double>::max();
	double max = -1.0 * std::numeric_limits<double>::max();

	// Remember: star[size - 1] = star[0].
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Figure out if the angle should be signed:
		// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

		// Get the tangent vector to the edge.
		Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
		if (e == NULL)
			std::cout << "OOPS" << std::endl;
		Vert* q = e->GetOtherVertex(v);
		glm::dvec3 te = q->GetPosition() - vPosition;
		if (q->index == v->index)
			std::cout << "OOPS" << std::endl;

		// DEBUG: check that the edge is correct.
		Triangle* t1 = e->GetOtherTriangle(star[i]);
		Triangle* t0 = e->GetOtherTriangle(star[i+1]);
		if (t1->index != star[i+1]->index || t0->index != star[i]->index)
			std::cout << "EDGE BROKE" << std::endl;

		// Check the inner product.
		double ip = glm::dot(glm::cross(current, next), te);
		if (ip > 0)
			angle *= -1.0;

		// Check for min or max.
		if (angle > max)
			max = angle;
		if (angle < min)
			min = angle;
	}
	return min * max;
}
double MeshAnalysis::ComputeMinPrincipalDistortion(Corner& c)
{
	// Get the vertex and its star.
	// Set the last element to be equal to the first so all the calculations can be done in a single for-loop.
	Vert* v = c.v;
	glm::dvec3 vPosition = v->GetPosition();
	std::vector<Triangle*> star = GetVertexStar(v);
	star.push_back(star[0]);

	//std::cout << "Current vertex is " << v->index << std::endl;
	if (c.o == NULL)
		std::cout << "BOUNDARY" << std::endl;

	// Min/max principal curvatures using the distortion.
	double min = std::numeric_limits<double>::max();

	// Remember: star[size - 1] = star[0].
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Figure out if the angle should be signed:
		// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

		// Get the tangent vector to the edge.
		Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
		if (e == NULL)
			std::cout << "OOPS" << std::endl;
		Vert* q = e->GetOtherVertex(v);
		glm::dvec3 te = q->GetPosition() - vPosition;
		if (q->index == v->index)
			std::cout << "OOPS" << std::endl;

		// DEBUG: check that the edge is correct.
		Triangle* t1 = e->GetOtherTriangle(star[i]);
		Triangle* t0 = e->GetOtherTriangle(star[i+1]);
		if (t1->index != star[i+1]->index || t0->index != star[i]->index)
			std::cout << "EDGE BROKE" << std::endl;

		// Check the inner product.
		double ip = glm::dot(glm::cross(current, next), te);
		if (ip > 0)
			angle *= -1.0;

		// Check for min or max.
		//if (angle > max)
		//	max = angle;
		if (angle < min)
			min = angle;
	}
	return min;
}

double MeshAnalysis::ComputeMaxPrincipalDistortion(Corner& c)
{
	// Get the vertex and its star.
	// Set the last element to be equal to the first so all the calculations can be done in a single for-loop.
	Vert* v = c.v;
	glm::dvec3 vPosition = v->GetPosition();
	std::vector<Triangle*> star = GetVertexStar(v);
	star.push_back(star[0]);

	//std::cout << "Current vertex is " << v->index << std::endl;
	if (c.o == NULL)
		std::cout << "BOUNDARY" << std::endl;

	// Min/max principal curvatures using the distortion.
	//double min = std::numeric_limits<double>::max();
	double max = -1.0 * std::numeric_limits<double>::max();

	// Remember: star[size - 1] = star[0].
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Figure out if the angle should be signed:
		// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

		// Get the tangent vector to the edge.
		Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
		if (e == NULL)
			std::cout << "OOPS" << std::endl;
		Vert* q = e->GetOtherVertex(v);
		glm::dvec3 te = q->GetPosition() - vPosition;
		if (q->index == v->index)
			std::cout << "OOPS" << std::endl;

		// DEBUG: check that the edge is correct.
		Triangle* t1 = e->GetOtherTriangle(star[i]);
		Triangle* t0 = e->GetOtherTriangle(star[i+1]);
		if (t1->index != star[i+1]->index || t0->index != star[i]->index)
			std::cout << "EDGE BROKE" << std::endl;

		// Check the inner product.
		double ip = glm::dot(glm::cross(current, next), te);
		if (ip > 0)
			angle *= -1.0;

		// Check for min or max.
		if (angle > max)
			max = angle;
		//if (angle < min)
		//	min = angle;
	}
	return max;
}

double MeshAnalysis::ComputeSignedDistortion(Corner& c)
{
	// Get the vertex and its star.
	// Set the last element to be equal to the first so all the calculations can be done in a single for-loop.
	Vert* v = c.v;
	glm::dvec3 vPosition = v->GetPosition();
	std::vector<Triangle*> star = GetVertexStar(v);
	star.push_back(star[0]);

	//std::cout << "Current vertex is " << v->index << std::endl;
	if (c.o == NULL)
		std::cout << "BOUNDARY" << std::endl;

	/*
	for (Triangle* t : star)
		t->Print();

	std::cout << std::endl;
	*/

	// Distortion is the signed perimeter of the Gauss map.
	double distortion = 0;

	// Remember: star[size - 1] = star[0].
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Figure out if the angle should be signed:
		// \ip{N_i \times N_{i+1} , t_e} > 0 => negative, where t_e is the tangent vector along the edge.

		// Get the tangent vector to the edge.
		Edge* e = MeshAnalysis::GetEdge(star[i], star[i+1]);
		if (e == NULL)
			std::cout << "OOPS" << std::endl;
		Vert* q = e->GetOtherVertex(v);
		glm::dvec3 te = q->GetPosition() - vPosition;
		if (q->index == v->index)
			std::cout << "OOPS" << std::endl;

		// DEBUG: check that the edge is correct.
		Triangle* t1 = e->GetOtherTriangle(star[i]);
		Triangle* t0 = e->GetOtherTriangle(star[i+1]);
		if (t1->index != star[i+1]->index || t0->index != star[i]->index)
			std::cout << "EDGE BROKE" << std::endl;

		// Check the inner product.
		double ip = glm::dot(glm::cross(current, next), te);
		if (ip > 0)
			angle *= -1.0;


		// Distortion.
		distortion += angle;
	}

	return distortion;
}
// Return the edge attached to both triangles.
Edge* MeshAnalysis::GetEdge(Triangle* t0, Triangle* t1)
{
	//t0->Print();
	//t1->Print();
	std::vector<Edge*> edges0 = t0->edges;
	std::vector<Edge*> edges1 = t1->edges;
	for (Edge* e0 : edges0)
	{
		for (Edge* e1 : edges1)
		{
			//std::cout << e0->index << " " << e1->index << std::endl;
			if (e0->index == e1->index)
			{
				//std::cout << "SUCCESS" << std::endl;
				return e0;
			}
		}
	}
	std::cout << "FAILURE" << std::endl;
	return NULL;
}

double MeshAnalysis::ComputeGaussianCone(Corner& c)
{
	// Get the vertex and its star.
	Vert* v = c.v;
	std::vector<Triangle*> star = GetVertexStar(v);

	// The dual cone is the mesh whose faces are spanned by the normal vectors of the vertex.
	// So the Gaussian curvature is the angle defect of this mesh at the vertex.
	double total = 0;
	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Gaussian curvature.
		total += angle;
	}
	// Last pair:
	glm::dvec3 current = star[star.size() - 1]->normal;
	glm::dvec3 next = star[0]->normal;

	// Angle between normals:
	double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
	double angle = acos(dot);
	total += angle;

	total = 2.0 * M_PI - total;
	return total;

	// Test.
	//double distortion = ComputeDistortion(c);
	//return distortion + total;

}

double MeshAnalysis::ComputeDistortion(Corner& c)
{
	// Get the vertex and its star.
	Vert* v = c.v;
	std::vector<Triangle*> star = GetVertexStar(v);

	// Get supplement of the angle between each pair of normal vectors.
	// This is the dihedral angle at the edge between normal vectors.
	double distortion = 0;

	for (int i = 0; i < star.size() - 1; ++i)
	{
		// Normals:
		glm::dvec3 current = star[i]->normal;
		glm::dvec3 next = star[i+1]->normal;

		// Angle between normals:
		double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
		double angle = acos(dot);

		// Distortion.
		double dihedral = M_PI - angle;
		distortion += M_PI - dihedral;
	}
	// Last angle:
	// Normals:
	glm::dvec3 current = star[star.size() - 1]->normal;
	glm::dvec3 next = star[0]->normal;

	// Angle between normals:
	double dot = glm::clamp(glm::dot(current, next), -1.0, 1.0);
	double angle = acos(dot);

	// Distortion.
	double dihedral = M_PI - angle;
	distortion += M_PI - dihedral;

	if (distortion < 0)
		std::cout << "Negative distortion." << std::endl;

	return distortion;
}


std::vector<Triangle*> MeshAnalysis::GetVertexStar(Vert* v)
{
	std::vector<Triangle*> star;

	// Bounce around CCW starting at any corner of the vertex.
	Corner* c = v->c;

	int k = -1;
	Corner* previous = c;
	while (k != c->index)
	{
		Corner* current = previous->p->o->p;
		k = current->index;
		star.push_back(current->t);
		previous = current;

		//std::cout << current->t->index << " " << 180 * current->angle / M_PI << std::endl;
	}
	return star;
}



double MeshAnalysis::ComputeHorizonMeasureTest(Corner& c)
{
	// Start with the mixed area.
	double mixedArea = ComputeMixedArea(c);
	if (mixedArea == -1)
		return 0;
	
	// Get the vertex associated with this corner.
	Vert* v = c.v;

	// Get list of triangle normal vectors in the list of triangles containing the vertex.
	std::vector<glm::dvec3> normals;
	for (Triangle* t : v->triangles)
	{
		normals.push_back(t->normal);
	}

	// Compute angles between each pair of normal vectors.
	double last = glm::clamp(glm::dot(normals[normals.size()-1], normals[0]), -1.0, 1.0);
	double total = acos(last);
	for (int i = 0; i < normals.size() - 1; ++i)
	{
		double dot = glm::clamp(glm::dot(normals[i], normals[i+1]), -1.0, 1.0);
		total += acos(dot);
	}

	// Testing: if the Gaussian curvature is negative, try adding 4pi.
	return 2.0 * total / mixedArea;
}
double MeshAnalysis::ComputeHorizonMeasure(Corner& c)
{
	// Start with the mixed perimeter.
	double mixedPerimeter = ComputeMixedPerimeter(c);
	if (mixedPerimeter == -1)
		return 0;
	
	// Get the vertex associated with this corner.
	Vert* v = c.v;

	// Get list of triangle normal vectors in the list of triangles containing the vertex.
	std::vector<glm::dvec3> normals;
	for (Triangle* t : v->triangles)
	{
		normals.push_back(t->normal);
	}

	// Compute angles between each pair of normal vectors.
	double last = glm::clamp(glm::dot(normals[normals.size()-1], normals[0]), -1.0, 1.0);
	double total = acos(last);
	for (int i = 0; i < normals.size() - 1; ++i)
	{
		double dot = glm::clamp(glm::dot(normals[i], normals[i+1]), -1.0, 1.0);
		total += acos(dot);
	}

	// Testing: if the Gaussian curvature is negative, try adding 4pi.
	return 2.0 * total / mixedPerimeter;
}

// Based on the paper, compute the ``mixed perimeter'' of the Voronoi area corresponding to the triangle for this corner.
// See notes for derivation of the arc lengths: use circumcenter and midpoint theorem for triangle segments.
double MeshAnalysis::ComputeMixedPerimeter(Corner& c)
{
	double mixedPerimeter = 0;
	Vert* v = c.v;

	// Bounce around the vertex using the corner:
	int k = -1;
	Corner* previous = &c;
	while (k != c.index)
	{
		// Check if this is a boundary vertex.
		// If so, set the mixed perimeter to be -1.
		if (previous->p->o == NULL)
			return -1;

		Corner* current = previous->p->o->p;
		k = current->index;

		// If the triangle is obtuse, but the obtuse angle is NOT at this corner:
		if (current->p->angle > 0.5 * M_PI ||
			current->n->angle > 0.5 * M_PI)
		{
			// The perimeter is then half of the length of the edge opposite the corner.
			mixedPerimeter += 0.5 * current->e->GetLength();
		}
		// If the triangle is obtuse and the angle IS at this corner:
		else if (current->angle > 0.5 * M_PI)
		{
			// The perimeter is then half the sum of the edges containing the vertex.
			// These edges are opposite the two adjacent corners of the current.
			mixedPerimeter += 0.5 * (current->p->e->GetLength() + current->n->e->GetLength());
		}
		// The triangle is not obtuse:
		else
		{
			// Actual Voronoi cell based upon the circumcenter.
			// First compute circumradius.
			double pEdgeLength = current->p->e->GetLength();
			double nEdgeLength = current->n->e->GetLength();
			double cEdgeLength = current->e->GetLength();
			double circumRadius = 0.25 * cEdgeLength * pEdgeLength * nEdgeLength / current->t->area;

			// Now compute the two arcs of the Voronoi cell.
			double pArcLength = std::sqrt(circumRadius * circumRadius - (0.25 * pEdgeLength * pEdgeLength));
			double nArcLength = std::sqrt(circumRadius * circumRadius - (0.25 * nEdgeLength * nEdgeLength));
			mixedPerimeter += pArcLength + nArcLength;
		}
		previous = current;
	}
	return mixedPerimeter;
}


double MeshAnalysis::GetVertexCurvature(Vert* v, Curvature curv)
{
	// Choose the correct curvature function.
	double (*CurvatureFunction)(Corner&);	
	if (curv == Curvature::GAUSSIAN)
		CurvatureFunction = ComputeGaussianCurvature;
	else if (curv == Curvature::MEAN)
		CurvatureFunction = ComputeMeanCurvature;
	else if (curv == Curvature::HORIZON)
		CurvatureFunction = ComputeHorizonMeasure;
	else if (curv == Curvature::ORIGINAL)
		CurvatureFunction = ComputeHorizonMeasureTest;
	else if (curv == Curvature::DISTORTION)
		CurvatureFunction = ComputeDistortion;
	else if (curv == Curvature::CONE)
		CurvatureFunction = ComputeGaussianCone;
	else if (curv == Curvature::DISTORTION_SIGNED)
		CurvatureFunction = ComputeSignedDistortion;
	else if (curv == Curvature::MEAN_SIGNED)
		CurvatureFunction = ComputeSignedMeanCurvature;
	else if (curv == Curvature::MAX_PRINCIPAL_DISTORTION)
		CurvatureFunction = ComputeMaxPrincipalDistortion;
	else if (curv == Curvature::MIN_PRINCIPAL_DISTORTION)
		CurvatureFunction = ComputeMinPrincipalDistortion;
	else if (curv == Curvature::FALSE_GAUSSIAN)
		CurvatureFunction = ComputeGaussianFromDistortion;
	else if (curv == Curvature::FALSE_MEAN)
		CurvatureFunction = ComputeMeanFromDistortion;
	else //if (curv == Curvature::PRINCIPAL_DEVIATION)
		CurvatureFunction = ComputePrincipalDeviation;

	Corner c = *(v->c);
	return CurvatureFunction(c);
}
std::vector<double> MeshAnalysis::GetVertexCurvatures(Polyhedron* p, Curvature curv)
{
	// Assume that curvatures[i] is the curvature of vertices[i].
	std::vector<double> curvatures(p->vlist.size(), std::numeric_limits<double>::max());

	// Choose the correct curvature function.
	double (*CurvatureFunction)(Corner&);	
	if (curv == Curvature::GAUSSIAN)
		CurvatureFunction = ComputeGaussianCurvature;
	else if (curv == Curvature::MEAN)
		CurvatureFunction = ComputeMeanCurvature;
	else if (curv == Curvature::HORIZON)
		CurvatureFunction = ComputeHorizonMeasure;
	else if (curv == Curvature::ORIGINAL)
		CurvatureFunction = ComputeHorizonMeasureTest;
	else if (curv == Curvature::DISTORTION)
		CurvatureFunction = ComputeDistortion;
	else if (curv == Curvature::CONE)
		CurvatureFunction = ComputeGaussianCone;
	else if (curv == Curvature::DISTORTION_SIGNED)
		CurvatureFunction = ComputeSignedDistortion;
	else if (curv == Curvature::MEAN_SIGNED)
		CurvatureFunction = ComputeSignedMeanCurvature;
	else if (curv == Curvature::MAX_PRINCIPAL_DISTORTION)
		CurvatureFunction = ComputeMaxPrincipalDistortion;
	else if (curv == Curvature::MIN_PRINCIPAL_DISTORTION)
		CurvatureFunction = ComputeMinPrincipalDistortion;
	else if (curv == Curvature::FALSE_GAUSSIAN)
		CurvatureFunction = ComputeGaussianFromDistortion;
	else if (curv == Curvature::FALSE_MEAN)
		CurvatureFunction = ComputeMeanFromDistortion;
	else //if (curv == Curvature::PRINCIPAL_DEVIATION)
		CurvatureFunction = ComputePrincipalDeviation;

	// Go through the corner list and compute the Gaussian curvature of the corresponding vertex.
	// Skip vertices we have already seen.
	for (Corner& c : p->clist)
	{
		int index = c.v->index;

		// Skip this vertex if we have already given it a value.
		if (curvatures[index] != std::numeric_limits<double>::max())
			continue;
		double curvature = CurvatureFunction(c);
		/*
		if (curv == Curvature::GAUSSIAN)
			curvature = abs(curvature);
		*/
		curvatures[index] = curvature;
	}
	return curvatures;
}

// See Figure 4 in the paper for the formulas used for mixedArea according to whether or not the triangles are obtuse.
double MeshAnalysis::ComputeMixedArea(Corner& c)
{
	double mixedArea = 0;
	Vert* v = c.v;

	// Bounce around the vertex using the corner:
	int k = -1;
	Corner* previous = &c;
	while (k != c.index)
	{
		// Check if this is a boundary vertex.
		// If so, set the mixed area to be -1.
		if (previous->p->o == NULL)
			return -1;

		Corner* adjacent = previous->p->o->p;
		k = adjacent->index;
		//std::cout << k << " " << c.index << " " << c.angle << " " << c.v->valence << " " << adjacent->v->index << std::endl;

		// If the triangle is obtuse, but the obtuse angle is NOT at this corner:
		if (adjacent->p->angle > 0.5 * M_PI ||
			adjacent->n->angle > 0.5 * M_PI)
		{
			//std::cout << "Obtuse triangle at " << c.v->index << std::endl;
			mixedArea += 0.25 * adjacent->t->area;	
		}
		// If the triangle is obtuse and the angle IS at this corner:
		else if (adjacent->angle > 0.5 * M_PI)
		{
			//std::cout << "Obtuse triangle at " << c.v->index << std::endl;
			mixedArea += 0.5 * adjacent->t->area;
		}
		// The triangle is not obtuse:
		else
		{
			double theta = adjacent->n->angle;
			double phi = previous->p->angle;

			// Doing a little trick: pq is equal to pr.
			double pq = adjacent->n->e->GetLength();
			double pr = previous->p->e->GetLength();
			pq *= pq;
			pr *= pr;

			double test = (1.0 / tan(theta)) + pr * (1.0 / tan(phi));
			if (test <= 0)
			{
				std::cout << "ERROR: non-positive mixed area. " << std::endl;
				return -1;
			}
			mixedArea += (1.0 / 8.0) * (pq * (1.0 / tan(theta)) + pr * (1.0 / tan(phi)));
		}
		previous = adjacent;
	}
	return mixedArea;
}

double MeshAnalysis::ComputeGaussianCurvature(Corner& c)
{
	/*
	double mixedArea = ComputeMixedArea(c);
	if (mixedArea == -1)
		return 0;
	return ((2.0 * M_PI) - c.v->totalAngle) / mixedArea;
	*/
	return ((2.0 * M_PI) - c.v->totalAngle);
}

// See Equation 8 in the paper.
double MeshAnalysis::ComputeSignedMeanCurvature(Corner& c)
{
	double mixedArea = ComputeMixedArea(c);
	
	// If this is a boundary vertex, just set the mean curvature to be zero:
	if (mixedArea == -1)
		return 0;

	Vert* v = c.v;
	glm::dvec3 total = glm::dvec3(0.0, 0.0, 0.0);

	// Bounce around the vertex using the corner:
	int k = -1;
	Corner* previous = &c;
	while (k != c.index)
	{
		Corner* adjacent = previous->p->o->p;
		k = adjacent->index;

		// If the triangle is obtuse, but the obtuse angle is NOT at this corner:
		double theta = adjacent->n->angle;
		double phi = adjacent->n->o->angle;

		Vert* w = previous->n->v;
		glm::dvec3 xi = glm::dvec3(v->x, v->y, v->z);
		glm::dvec3 xj = glm::dvec3(w->x, w->y, w->z);
		glm::dvec3 difference = xi - xj;

		total += MeanCurvatureWeight(theta, phi) * difference;
		previous = adjacent;
	}
	//return 0.5 * ((double)1.0 / (2.0 * mixedArea)) * total;
	glm::dvec3 mcVector = ((double)1.0 / (2.0 * mixedArea)) * total;

	double meanCurvature = 0.5 * glm::length(mcVector);
	if (glm::dot(v->normal, mcVector) > 0)
		return meanCurvature;
	else
		return -1.0 * meanCurvature;
}
double MeshAnalysis::ComputeMeanCurvature(Corner& c)
{
	double mixedArea = ComputeMixedArea(c);
	
	// If this is a boundary vertex, just set the mean curvature to be zero:
	if (mixedArea == -1)
		return 0;

	Vert* v = c.v;
	glm::dvec3 total = glm::dvec3(0.0, 0.0, 0.0);

	// Bounce around the vertex using the corner:
	int k = -1;
	Corner* previous = &c;
	while (k != c.index)
	{
		Corner* adjacent = previous->p->o->p;
		k = adjacent->index;

		// If the triangle is obtuse, but the obtuse angle is NOT at this corner:
		double theta = adjacent->n->angle;
		double phi = adjacent->n->o->angle;

		Vert* w = previous->n->v;
		glm::dvec3 xi = glm::dvec3(v->x, v->y, v->z);
		glm::dvec3 xj = glm::dvec3(w->x, w->y, w->z);
		glm::dvec3 difference = xi - xj;

		total += MeanCurvatureWeight(theta, phi) * difference;
		previous = adjacent;
	}
	//return 0.5 * ((double)1.0 / (2.0 * mixedArea)) * total;
	glm::dvec3 mcVector = ((double)1.0 / (2.0 * mixedArea)) * total;

	/*
	double meanCurvature = glm::length(mcVector);
	if (glm::dot(v->normal, mcVector) > 0)
		return meanCurvature;
	else
		return -1 * meanCurvature;
		*/

	return 0.5 * glm::length(mcVector);
}

double MeshAnalysis::MeanCurvatureWeight(double theta, double phi)
{
	return (1.0 / tan(theta)) + (1.0 / tan(phi));
}




void MeshAnalysis::GetCornerList(Polyhedron* p)
{
	std::vector<Triangle>& tlist = p->tlist;
	int numTriangles = tlist.size();
	std::vector<Corner>& corners = p->clist;

	// Analyze one triangle at a time.
	int cornerIndex = 0;
	for (int i = 0; i < numTriangles; ++i)
	{
		// Get the triangle and its vertices.
		Triangle& t = tlist[i];
		Vert* v1 = t.vertices[0];
		Vert* v2 = t.vertices[1];
		Vert* v3 = t.vertices[2];

		// Corners.
		Corner& c1 = corners[cornerIndex];
		c1.index = cornerIndex;
		Corner& c2 = corners[cornerIndex + 1];
		c2.index = cornerIndex + 1;
		Corner& c3 = corners[cornerIndex + 2];
		c3.index = cornerIndex + 2;
		 
		// Triangle: c.t.
		c1.t = &t;
		c2.t = &t;
		c3.t = &t;

		// Vertex: c.v.
		c1.v = v1;
		c2.v = v2;
		c3.v = v3;
		v1->c = &c1;
		v2->c = &c2;
		v3->c = &c3;

		// Next: c.n.
		c1.n = &c2;
		c2.n = &c3;
		c3.n = &c1;

		// Previous: c.p.
		c1.p = &c3;
		c2.p = &c1;
		c3.p = &c2;

		// Edge: c.e.
		for (int j = 0; j < 3; ++j)
		{
			// For each vertex, check to see if that vertex is NOT contained in an edge.
			// The edge that does not contain the vertex is the edge that we want for c.e.
			Edge* e = t.edges[j];
			if (e->Contains(v1) == -1)
			{
				c1.e = e;
			}
			if (e->Contains(v2) == -1)
			{
				c2.e = e;
			}
			if (e->Contains(v3) == -1)
			{
				c3.e = e;
			}
		}

		// And now the hard part...
		// Opposite: c.o.
		for (int j = 0; j < 3; ++j)
		{
			Corner& c = corners[cornerIndex + j];

			// Get the edge of the corner and find the triangle that is not equal to c.t.
			Edge* e = c.e;

			// If this edge is attached to only one triangle then there is no opposite corner.
			if (!(e->isBoundary()))
			{
				// Pick the correct triangle that is NOT equal to the current triangle c.t.	
				Triangle* s = e->GetOtherTriangle(&t);

				// Look through the vertices of the triangle and find the one that is not in the shared edge.
				for (int k = 0; k < 3; ++k)
				{
					int triangleVertexIndex = k;
					Vert* w = s->vertices[triangleVertexIndex];
					if (e->Contains(w) == -1)
					{
						// The annoying part.
						// We need to figure out what the index of the desired corner c.o will be, before it is even initialized.
						// Since we are creating these corners by iterating through the triangles, we can determine the future index of c.o.
						// First, each triangle generates three corners.
						// So if we are looking at the ith triangle, then the desired corner will have index 3i, 3i+1, or 3i+2.
						int index = 3 * s->index + triangleVertexIndex;
						c.o = &corners[index];
						//c.Print();
						break;
					}
				}
			}
			else
			{
				c.o = NULL;
			}
		}

		cornerIndex += 3;
	}
}

void MeshAnalysis::GetValenceDeficit(Polyhedron* p)
{
	// We can cycle around a vertex using corners.
	// Given a corner, check c.p.o.p to get a new corner at the same vertex.
	// Hop around in this fashion until we get back to where we started.

	// WARNING: KNOWN BUG:
	// Will crash if the mesh has boundary.
	// Something goes wrong when bouncing around using the corners if there is boundary.
	// GO FIX!

	// This method requires that the polyhedron has a corner table.
	if (p->clist.size() < 1)
	{
		GetCornerList(p);
	}
	std::vector<Corner>& corners = p->clist;

	for (Corner c : corners)
	{
		// Valence:
		// Don't bother computing if the valence of this vertex has already been set.
		if (c.v->valence == 0)
		{
			// Bounce around until we get back to where we started OR until we reach a boundary.
			int index = -1;
			int valence = 0;
			bool goPrevious = true;
			Corner* previous = &c;
			
			// Valence computation:
			while (index != c.index)
			{
				// Go the correct direction.
				Corner* rotate = goPrevious ? previous->p : previous->n;

				// The c.o.p corner will be NULL if the starting triangle was a boundary.
				// So check this first:
				if (rotate->o != NULL)
				{
					// Still make sure to go the correct direction.
					Corner* adjacent = goPrevious ? rotate->o->p : rotate->o->n;
					index = adjacent->index;

					++valence;
					previous = adjacent;
				}
				// NULL (boundary) case:
				else
				{
					// Still increase the valence.
					++valence;

					// Change the direction. 
					// But if we already did try going the other direction, we are done.
					if (goPrevious)
					{
						previous = &c;
						goPrevious = false;
					}
					else
						index = c.index;
				}
			}
			c.v->valence = valence;
			int deficit = 6 - valence;
			p->valenceDeficit += deficit;
		}
	}
	/*
	for (Corner c : corners)
	{
		// Valence:
		// Don't bother computing if the valence of this vertex has already been set.
		if (c.v->valence == 0)
		{
			// Bounce around until we get back to where we started.
			int index = -1;
			int valence = 0;
			Corner* previous = &c;
			while (index != c.index)
			{
				// Valence computation:
				Corner* adjacent = previous->p->o->p;
				index = adjacent->index;

				valence++;
				previous = adjacent;
			}
			c.v->valence = valence;
			int deficit = 6 - valence;
			p->valenceDeficit += deficit;
		}
	}
	*/
}

void MeshAnalysis::ComputeAngle(Corner& c)
{
	glm::dvec3 cWorld = glm::dvec3(c.v->x, c.v->y, c.v->z);
	glm::dvec3 cnWorld = glm::dvec3(c.n->v->x, c.n->v->y, c.n->v->z);
	glm::dvec3 cpWorld = glm::dvec3(c.p->v->x, c.p->v->y, c.p->v->z);
	glm::dvec3 cn = cnWorld - cWorld;
	glm::dvec3 cp = cpWorld - cWorld;

	double dot = glm::clamp(glm::dot(cn, cp), -1.0, 1.0);
	//double dot = glm::dot(cn, cp);
	double cnLength = glm::length(cn);
	double cpLength = glm::length(cp);
	double angle = acos(dot / (cnLength * cpLength));
	c.angle = angle;
}

void MeshAnalysis::ComputeAngles(std::vector<Corner>& corners)
{
	for (Corner& c : corners)
	{
		// Check to see if the angle has already been computed. If not, continue:
		if (c.angle == 0)
		{
			ComputeAngle(c);

			// Add this angle to the vertex total angle.
			c.v->totalAngle += c.angle;

			// Add this angle to the corresponding triangle angle.
			// First get the index:
			//int k = c.t->Contains(c.v);
			//c.t->angles[k] = c.angle;
		}
	}
}

void MeshAnalysis::GetAngleDeficit(Polyhedron* p)
{
	ComputeAngles(p->clist);
	double totalAngleDeficit = 0;
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		double vertexAngle = p->vlist[i].totalAngle;
		double angleDeficit = 2 * M_PI - vertexAngle;
		totalAngleDeficit += angleDeficit;
	}
	p->angleDeficit = totalAngleDeficit;
}

float MeshAnalysis::InverseLerp(float start, float end, float v)
{
	return (v - start) / (end  - start);
}
