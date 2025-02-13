#include "curvature.hpp"
#include <Eigen/src/Eigenvalues/EigenSolver.h>


// See Figure 4 in the paper for the formulas used for mixedArea according to whether or not the triangles are obtuse.
double Curvature::ComputeMixedArea(Corner& c)
{
	double mixedArea = 0;
	Vert* v = c.v;

	// Bounce around the vertex using the corner:
	int k = -1;
	Corner* previous = &c;
	while (k != c.index)
	{
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
			//double theta = adjacent->n->angle;
			//double phi = previous->p->angle;
			double theta = adjacent->n->angle;
			double phi = adjacent->p->angle;

			double pq = adjacent->n->e->length;
			double pr = adjacent->p->e->length;
			pq *= pq;
			pr *= pr;
			/*
			Vert* w = previous->n->v;
			glm::dvec3 xi = glm::dvec3(v->x, v->y, v->z);
			glm::dvec3 xj = glm::dvec3(w->x, w->y, w->z);
			double squareMagnitude = glm::length2(xi - xj);
			*/

			mixedArea += (1.0 / 8.0) * (pq * (1.0 / tan(theta)) + pr * (1.0 / tan(phi)));
			//mixedArea += 0.25 * MeanCurvatureWeight(theta, phi) * squareMagnitude;
			//mixedArea += MeanCurvatureWeight(theta, phi) * squareMagnitude;
		}
		previous = adjacent;
	}
	return mixedArea;
}

// See Equation 8 in the paper.
glm::dvec3 Curvature::ComputeMeanCurvature(Corner& c)
{
	double mixedArea = ComputeMixedArea(c);
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
	return ((double)1.0 / (2.0 * mixedArea)) * total;
}

double Curvature::ComputeGaussianCurvature(Corner& c)
{
	double mixedArea = ComputeMixedArea(c);
	return ((2.0 * M_PI) - c.v->totalAngle) / mixedArea;
	//return 1.0/(((2.0 * M_PI) - c.v->totalAngle) / mixedArea);
}


double Curvature::ComputeNormalCurvature(Vert* v1, Vert* v2)
{
	glm::dvec3 xi = glm::dvec3(v1->x, v1->y, v1->z);
	glm::dvec3 xj = glm::dvec3(v2->x, v2->y, v2->z);
	glm::dvec3 difference = xi - xj;
	double squareMagnitude = glm::length2(difference);
	//std::cout << 2.0 * glm::dot(difference, v1->normal) / squareMagnitude << " " << glm::to_string(v1->normal) << " " << glm::to_string(xi) << " " << glm::to_string(xj) << std::endl;
	return 2.0 * glm::dot(difference, v1->normal) / squareMagnitude;
}

TangentSpace Curvature::GetCurvatureTensor(Corner& c)
{
	// Go through the connected vertices of v and project the edges onto the tangent plane of v.
	// The tangent plane will just be determined by the normal vector at v.
	// We will need to establish a coordinate system.
	Vert* v = c.v;
	std::vector<Vert*> connectedVertices = c.GetAdjacentVertices();

	// We will establish an orthonormal coordinate system on the tangent space.
	// We'll just pick out the first edge from the connected vertices and project that vector onto the space and call it e1.
	glm::dvec3 firstEdge = connectedVertices[0]->getPosition() - v->getPosition();
	TangentSpace tps(v, firstEdge);

	/*
	std::cout << "c.v.position: " << glm::to_string(v->getPosition()) << std::endl;
	std::cout << "w.position: " << glm::to_string(connectedVertices[0]->getPosition()) << std::endl;
	std::cout << "first edge: " << glm::to_string(firstEdge) << std::endl;
	std::cout << "e1: " << glm::to_string(tps.e1) << std::endl;
	std::cout << "e2: " << glm::to_string(tps.e2) << std::endl;
	std::cout << "Normal: " << glm::to_string(v->normal) << std::endl;
	*/

	// Now for the hard part: we need to build the linear system given by the normal curvatures:
	// Ax = b:
	Eigen::VectorXd normalCurvatureVector(connectedVertices.size()); // the vector b.
	Eigen::MatrixXd coordinateMatrix(connectedVertices.size(), 3);	 // the matrix A.

	// Fill the linear system:
	for (int i = 0; i < connectedVertices.size(); ++i)
	{
		Vert* w = connectedVertices[i];
		normalCurvatureVector(i) = ComputeNormalCurvature(v, w);

		// Convert the edge vector into local coordinates: (a, b) relative to {e1, e2}.
		glm::dvec2 local = tps.LocalCoordinates(w->getPosition());
		/*
		std::cout << std::endl;
		std::cout << "Converting the edge connecting v to " << glm::to_string(w->getPosition()) << std::endl;
		std::cout << glm::to_string(local) << std::endl;
		std::cout << std::endl;
		*/

		// Expression of curvature tensor in {e1, e2} coordinates:
		coordinateMatrix(i, 0) = local[0] * local[0];
		coordinateMatrix(i, 1) = 2.0 * local[0] * local[1];
		coordinateMatrix(i, 2) = local[1] * local[1];
	}

	//std::cout << "Matrix: " << coordinateMatrix << std::endl;
	//std::cout << "Vector: " << normalCurvatureVector << std::endl;
	
	// Solve using Eigen:
	Eigen::Vector3d solution = coordinateMatrix.colPivHouseholderQr().solve(normalCurvatureVector);
	//std::cout << "Solution: " << solution << std::endl;

	// Build up curvature tensor matrix:
	Eigen::Matrix2d curvatureTensor;
	curvatureTensor(0, 0) = solution[0];
	curvatureTensor(1, 0) = solution[1];
	curvatureTensor(0, 1) = solution[1];
	curvatureTensor(1, 1) = solution[2];
	tps.curvatureTensor = curvatureTensor;

	/*
	// Get eigenvalues and eigenvectors:
	Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> es(curvatureTensor);
	Eigen::Vector2d eigenValues = es.eigenvalues();
	Eigen::MatrixXd eigenVectors = es.eigenvectors();
	//std::cout << "Eigenvalues: " << eigenValues << std::endl;
	//std::cout << "Eigenvectors: " << eigenVectors << std::endl;
	//std::cout << std::endl;

	// Set them in the tangent space:
	tps.k1 = eigenValues.x();
	//std::cout << "Set k1" << std::endl;
	tps.p1 = eigenVectors.col(0);
	//std::cout << "Set p1" << std::endl;
	tps.k2 = eigenValues.y();
	//std::cout << "Set k2" << std::endl;
	tps.p2 = eigenVectors.col(1);
	//std::cout << "Set p2" << std::endl;
	*/
	

	// Get the global curvature tensor.
	tps.curvatureTensorGlobal = tps.GlobalCoordinates(curvatureTensor);	

	return tps;
}

void Curvature::SmoothCurvatureTensor(Polyhedron* p, std::vector<TangentSpace>& tangentSpaces, double dt, int iterations)
{
	// Assuming that the curvature tensors have been found in the tangent spaces, ordered by vertex index.
	
	// Loop through all 9 entries of the global curvature tensor.
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			// Assign value0 to this value of the curvature tensor to each vertex:
			for (Vert& v : p->vlist)
			{
				v.value0 = tangentSpaces[v.index].curvatureTensorGlobal(i, j);
			}

			// Apply smoothing:
			Smoothing::SmoothValueMeanCurvature(p, dt, iterations);

			// Assign the smoothed value of the curvature tensor to the original matrix:
			for (Vert& v : p->vlist)
			{
				tangentSpaces[v.index].curvatureTensorGlobal(i, j) = v.value0;
			}
		}
	}

	// Smoothing has been done, so now get the smoothed curvature tensor back into local coordinates... the brute force way.
	for (TangentSpace& tps : tangentSpaces)
	{
		tps.curvatureTensor = tps.LocalCoordinates(tps.curvatureTensorGlobal);
	}
}

void Curvature::GetEigenSpaces(std::vector<TangentSpace>& tangentSpaces)
{
	for (TangentSpace& tps : tangentSpaces)
	{
		// Get eigenvalues and eigenvectors:
		Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> es(tps.curvatureTensor);
		Eigen::Vector2d eigenValues = es.eigenvalues();
		Eigen::MatrixXd eigenVectors = es.eigenvectors();
		//std::cout << "Eigenvalues: " << eigenValues << std::endl;
		//std::cout << "Eigenvectors: " << eigenVectors << std::endl;
		//std::cout << std::endl;

		// Set them in the tangent space:
		tps.k1 = eigenValues.x();
		//std::cout << "Set k1" << std::endl;
		tps.p1 = eigenVectors.col(0);
		//std::cout << "Set p1" << std::endl;
		tps.k2 = eigenValues.y();
		//std::cout << "Set k2" << std::endl;
		tps.p2 = eigenVectors.col(1);
		//std::cout << "Set p2" << std::endl;
	}
}

void Curvature::GetTriangleStreamlines(Polyhedron* p, std::vector<TangentSpace>& tangentSpaces, std::vector<Eigen::Vector3d>& minorStream, std::vector<Eigen::Vector3d>& majorStream)
{
	// Go through the triangles and assign a streamvector by taking the weighted average of the principal directions of the vertices.
	// The weights are determined by the Voronoi area, using the ``mixed area'' computation.
	// We will have to convert all prinicpal directions to the global frame.
	for (Triangle& t : p->tlist)
	{
		Eigen::Vector3d minorTriangle;
		minorTriangle(0) = 0;
		minorTriangle(1) = 0;
		minorTriangle(2) = 0;
		Eigen::Vector3d majorTriangle;
		majorTriangle(0) = 0;
		majorTriangle(1) = 0;
		majorTriangle(2) = 0;

		// Quick check if the triangle is obtuse:
		int obtuseIndex = -1;
		for (int i = 0; i < 3; ++i)
		{
			if (t.angles[i] > 0.5 * M_PI)
				obtuseIndex = i;
		}

		for (int i = 0; i < 3; ++i)
		{
			Vert* v = t.vertices[i];
			TangentSpace& tps = tangentSpaces[v->index];

			// Principal directions:
			Eigen::Vector2d minorLocal = tps.p1;
			Eigen::Vector2d majorLocal = tps.p2;
			Eigen::Vector3d minorGlobal = tps.GlobalCoordinates(minorLocal);
			Eigen::Vector3d majorGlobal = tps.GlobalCoordinates(majorLocal);

			// Voronoi area:
			double mixedArea = 0;

			// Check if the angle here is obtuse:
			if (obtuseIndex == i)
				mixedArea = 0.5 * t.area;
			else if (obtuseIndex > -1)
				mixedArea = 0.25 * t.area;
			else
			{
				// Not obtuse. Proceed as normal:
				double q = t.angles[(i + 1) % 3];
				double r = t.angles[(i + 2) % 3];

				double pr = t.edges[(i + 2) % 3]->length;
				double pq = t.edges[i]->length;
				pr *= pr;
				pq *= pq;

				mixedArea = (1.0 / 8.0) * (pr * (1.0 / tan(q)) + pq * (1.0 / tan(r)));
			}
			
			// Now add these to the triangle directions:
			minorTriangle += (mixedArea / t.area) * minorGlobal;
			majorTriangle += (mixedArea / t.area) * majorGlobal;
		}

		minorStream[t.index] = minorTriangle.normalized();
		majorStream[t.index] = majorTriangle.normalized();
	}
}














double Curvature::MeanCurvatureWeight(double theta, double phi)
{
	return 0.5 * ((1.0 / tan(theta)) + (1.0 / tan(phi)));
}

Curvature::Curvature(){}
Curvature::~Curvature(){}
