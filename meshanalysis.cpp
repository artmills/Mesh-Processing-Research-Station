#include "meshanalysis.hpp"


MeshAnalysis::MeshAnalysis() {}
MeshAnalysis::~MeshAnalysis() {}

float MeshAnalysis::GetHorizonArea(Vertex v0, Vertex v1, Vertex v2)
{
	glm::vec3 n0 = v0.getNormal();
	glm::vec3 n1 = v1.getNormal();
	glm::vec3 n2 = v2.getNormal();

	float d0 = std::clamp(glm::dot(n1, n2), -1.0f, 1.0f);
	float d1 = std::clamp(glm::dot(n0, n2), -1.0f, 1.0f);
	float d2 = std::clamp(glm::dot(n0, n1), -1.0f, 1.0f);

	float s = 2.0f * (acos(d0) + acos(d1) + acos(d2)); 
	if (s != s)
	{
		std::cout << d2 << " " << d1 << " " << d2 << " " << acos(d0) << " " << acos(d1) << " " << acos(d2) << std::endl;
	}
	return 2.0f * (acos(d0) + acos(d1) + acos(d2));
}
double MeshAnalysis::GetHorizonArea(Triangle t)
{
	glm::dvec3 v1 = t.vertices[0]->normal;
	glm::dvec3 v2 = t.vertices[1]->normal;
	glm::dvec3 v3 = t.vertices[2]->normal;

	double d12 = std::clamp(glm::dot(v1, v2), -1.0, 1.0);
	double d23 = std::clamp(glm::dot(v2, v3), -1.0, 1.0);
	double d31 = std::clamp(glm::dot(v3, v1), -1.0, 1.0);

	glm::dvec3 cross12 = glm::cross(v1, v2);
	double det12 = glm::length(cross12);
	double angle12 = atan2(det12, d12);
	double acute12 = std::min(angle12, (double)abs(M_PI - angle12));

	glm::dvec3 cross23 = glm::cross(v2, v3);
	double det23 = glm::length(cross23);
	double angle23 = atan2(det23, d23);
	double acute23 = std::min(angle23, (double)abs(M_PI - angle23));

	glm::dvec3 cross31 = glm::cross(v3, v1);
	double det31 = glm::length(cross31);
	double angle31 = atan2(det31, d31);
	double acute31 = std::min(angle31, (double)abs(M_PI - angle31));

	//return 2.0 * (angle12 + angle23 + angle31);
	return 2.0 * (acute12 + acute23 + acute31);

	/*
	return 2.0 * (acos(d1) + acos(d2) + acos(d3));
	*/


}

std::vector<double> MeshAnalysis::GetHorizonMeasureLength(Polyhedron* p)
{
	std::vector<double> ratios;
	for (Triangle& t : p->tlist)
	{
		double horizonArea = GetHorizonArea(t);

		// Compute perimeter of the triangle.
		glm::dvec3 p1 = (glm::dvec3)(t.vertices[0]->x, t.vertices[0]->y, t.vertices[0]->z);
		glm::dvec3 p2 = (glm::dvec3)(t.vertices[1]->x, t.vertices[1]->y, t.vertices[1]->z);
		glm::dvec3 p3 = (glm::dvec3)(t.vertices[2]->x, t.vertices[2]->y, t.vertices[2]->z);
		double l1 = glm::length(p1 - p2);
		double l2 = glm::length(p2 - p3);
		double l3 = glm::length(p3 - p1);
		
		/*
		double s = horizonArea / (l1 + l2 + l3);
		std::cout << s << std::endl;
		if (s != s)
		{
			std::cout << horizonArea << " " << l1 + l2 + l3 << " " << horizonArea / (l1 + l2 + l3) << std::endl;
		}
		*/
		ratios.push_back(horizonArea / (l1 + l2 + l3));
	}

	// Now analyze statistics:
	/*
	double sum = 0;
	double max = 0;
	double min = std::numeric_limits<double>::max();
	//double zeroes = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		double x = ratios[i];
		sum += x;	
		if (x > max)
			max = x;
		if (x < min)
			min = x;
		//if (x == 0)
		//	zeroes++;
	}
	double mean = sum / (float)ratios.size();

	double stdSums = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		stdSums += (ratios[i] - mean) * (ratios[i] - mean);
	}
	double standardDeviation = sqrt((1.0f / (float)ratios.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Length(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;
	std::cout << std::endl;
	*/
	return ratios;
}

void MeshAnalysis::HorizonMeasureLength(std::vector<MeshComponent>& meshes)
{
	std::vector<double> ratios;

	for (MeshComponent mesh : meshes)
	{
		std::vector<Vertex>& vertices = mesh.getVertices();
		std::vector<uint>& triangles = mesh.getTriangles();

		for (int i = 0; i < triangles.size(); i += 3)
		{
			// Get vertices of this triangle.
			Vertex p0 = vertices[triangles[i + 0]];
			Vertex p1 = vertices[triangles[i + 1]];
			Vertex p2 = vertices[triangles[i + 2]];

			// Get horizon area of the triangle.
			float horizonArea = GetHorizonArea(p0, p1, p2);

			// Compute perimeter of the triangle.
			double l0 = glm::length(p0.getPosition() - p1.getPosition());
			double l1 = glm::length(p1.getPosition() - p2.getPosition());
			double l2 = glm::length(p2.getPosition() - p0.getPosition());
			
			ratios.push_back(horizonArea / (l0 + l1 + l2));
		}
		std::cout << "Done analyzing one mesh. " << std::endl;
	}

	// Now analyze statistics:
	float sum = 0;
	float max = 0;
	float min = std::numeric_limits<float>::max();
	//double zeroes = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		float x = ratios[i];
		sum += x;	
		if (x > max)
			max = x;
		if (x < min)
			min = x;
		//if (x == 0)
		//	zeroes++;
	}
	float mean = sum / (float)ratios.size();

	float stdSums = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		stdSums += (ratios[i] - mean) * (ratios[i] - mean);
	}
	float standardDeviation = sqrt((1.0f / (float)ratios.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Length(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;
	std::cout << std::endl;

	// Go back through the meshes and assign colors to vertices according to the statistics.
	int meshCounter = 0;
	for (MeshComponent& mesh : meshes)
	{
		for (int i = 0; i < mesh.getVertices().size(); ++i)
		{
			Vertex& v = mesh.getVertices()[i];

			// Look up the horizon measure of this vertex.
			float horizon = ratios[i + meshCounter];

			// Assign color to this vertex.
			double distFromMean = abs(horizon - mean);
			double percent = InverseLerp(min, max, distFromMean);
			percent = cbrt(percent);

			v.setColor(glm::vec4(1.0f, 1.0f - percent, 1.0f - percent, 1.0f));
		}
		meshCounter += mesh.getVertices().size();
	}
}

void MeshAnalysis::HorizonMeasureLength(Polyhedron* p)
{
	std::vector<double> ratios;
	for (Triangle& t : p->tlist)
	{
		double horizonArea = GetHorizonArea(t);

		// Compute perimeter of the triangle.
		glm::dvec3 p1 = (glm::dvec3)(t.vertices[0]->x, t.vertices[0]->y, t.vertices[0]->z);
		glm::dvec3 p2 = (glm::dvec3)(t.vertices[1]->x, t.vertices[1]->y, t.vertices[1]->z);
		glm::dvec3 p3 = (glm::dvec3)(t.vertices[2]->x, t.vertices[2]->y, t.vertices[2]->z);
		double l1 = glm::length(p1 - p2);
		double l2 = glm::length(p2 - p3);
		double l3 = glm::length(p3 - p1);
		
		ratios.push_back(horizonArea / (l1 + l2 + l3));
	}

	// Now analyze statistics:
	double sum = 0;
	double max = 0;
	double min = std::numeric_limits<double>::max();
	//double zeroes = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		double x = ratios[i];
		sum += x;	
		if (x > max)
			max = x;
		if (x < min)
			min = x;
		//if (x == 0)
		//	zeroes++;
	}
	double mean = sum / (float)ratios.size();

	double stdSums = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		stdSums += (ratios[i] - mean) * (ratios[i] - mean);
	}
	double standardDeviation = sqrt((1.0f / (float)ratios.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Length(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;
	std::cout << std::endl;
	//std::cout << "The number of points with zero horizon measure are: " << zeroes << std::endl;
}

void MeshAnalysis::HorizonMeasureAnalysis(Polyhedron* p)
{
	std::vector<double> ratios;
	for (Triangle& t : p->tlist)
	{
		double horizonArea = GetHorizonArea(t);
		ratios.push_back(horizonArea / t.area);
	}

	// Now analyze statistics:
	double sum = 0;
	double max = 0;
	double min = std::numeric_limits<double>::max();
	//double zeroes = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		double x = ratios[i];
		sum += x;	
		if (x > max)
			max = x;
		if (x < min)
			min = x;
		//if (x == 0)
		//	zeroes++;
	}
	double mean = sum / (float)ratios.size();

	double stdSums = 0;
	for (int i = 0; i < ratios.size(); ++i)
	{
		stdSums += (ratios[i] - mean) * (ratios[i] - mean);
	}
	double standardDeviation = sqrt((1.0f / (float)ratios.size()) * stdSums);

	std::cout << "Statistics for: Area(H_V) / Area(V): " << std::endl;
	std::cout << "The mean is " << mean << ". " << std::endl;
	std::cout << "The standard deviation is " << standardDeviation << ". " << std::endl;
	std::cout << "The max is " << max << ". " << std::endl;
	std::cout << "The min is " << min << ". " << std::endl;
	std::cout << std::endl;
	//std::cout << "The number of points with zero horizon measure are: " << zeroes << std::endl;
}

double MeshAnalysis::GetHorizonMeasure(Triangle t)
{
	glm::dvec3 v1 = t.vertices[0]->normal;
	glm::dvec3 v2 = t.vertices[1]->normal;
	glm::dvec3 v3 = t.vertices[2]->normal;

	double d1 = glm::dot(v2, v3);
	double d2 = glm::dot(v1, v3);
	double d3 = glm::dot(v1, v2);
	return (acos(d1) + acos(d2) + acos(d3)) / (2.0 * M_PI);
}

std::vector<double> MeshAnalysis::ComputeHorizonMeasure(Polyhedron* p)
{
	// Since the horizon measure is computed per-triangle we will average the horizon measure on each triangle.
	std::vector<double> areas(p->vlist.size()); // (key, value) = (index, horizon area).
	for (Triangle& t : p->tlist)
	{
		// Compute the horizon measure and add it to the map of horizon measures.
		double horizonMeasure = GetHorizonMeasure(t);
		areas[t.vertices[0]->index] += horizonMeasure;
		areas[t.vertices[1]->index] += horizonMeasure;
		areas[t.vertices[2]->index] += horizonMeasure;
	}

	// Now take the average of the horizon measures computed so far.
	for (Vert& v : p->vlist)
	{
		areas[v.index] /= v.valence;
	}	

	return areas;
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
			if (e->numberOfTriangles > 1)
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
}

void MeshAnalysis::ComputeAngle(Corner& c)
{
	glm::dvec3 cWorld = glm::dvec3(c.v->x, c.v->y, c.v->z);
	glm::dvec3 cnWorld = glm::dvec3(c.n->v->x, c.n->v->y, c.n->v->z);
	glm::dvec3 cpWorld = glm::dvec3(c.p->v->x, c.p->v->y, c.p->v->z);
	glm::dvec3 cn = cnWorld - cWorld;
	glm::dvec3 cp = cpWorld - cWorld;

	double dot = glm::dot(cn, cp);
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
		//std::cout << vertexAngle << std::endl;
		double angleDeficit = 2 * M_PI - vertexAngle;
		totalAngleDeficit += angleDeficit;
	}
	p->angleDeficit = totalAngleDeficit;
}

float MeshAnalysis::InverseLerp(float start, float end, float v)
{
	return (v - start) / (end  - start);
}
