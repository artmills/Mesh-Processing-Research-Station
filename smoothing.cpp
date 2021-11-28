#include "smoothing.hpp"


Smoothing::Smoothing() {}
Smoothing::~Smoothing() {}

void Smoothing::SmoothMesh(Polyhedron* p, double dt, Weight type)
{
	// If the choice of weight is STATIC, we will need a copy of the original vertices.
	std::vector<Vert> originalVertices;
	if (type == Weight::CORD_STATIC || type == Weight::MEAN_CURVATURE_STATIC || type == Weight::MEAN_VALUE_STATIC)
	{
		originalVertices.reserve(p->vlist.size());
		for (int i = 0; i < p->vlist.size(); ++i)
		{
			originalVertices.push_back(p->vlist[i]);
		}
	}
	// Go through the corners of p and recompute the positions.
	// Keep track of which vertices we have seen already.
	std::map<int, Vert*> checked;
	for (int i = 0; i < p->clist.size(); ++i)
	{
		Corner& c = p->clist[i];

		// Check if we have seen this vertex before.
		if (!checked.count(c.v->index))
		{
			// Get all connected vertices and update the map.
			Vert* v = c.v;
			std::vector<Vert*> connected = c.GetAdjacentVertices();
			checked.insert({ v->index, v });
			//std::cout << "Original coordinates: " << v->x << " " << v->y << " " << v->z << std::endl;

			double xSum = 0;
			double ySum = 0;
			double zSum = 0;
			double weightCoefficient = 0.0;
			
			switch(type)
			{
				case Weight::UNIFORM:
					{
						for (int j = 0; j < connected.size(); ++j)
						{
							weightCoefficient = UniformWeight(connected.size());
							
							xSum += weightCoefficient * (connected[j]->x - v->x);
							ySum += weightCoefficient * (connected[j]->y - v->y);
							zSum += weightCoefficient * (connected[j]->z - v->z);
						}
						break;
					}

				case Weight::CORD_DYNAMIC:
					{
						// Compute denominator of weight w_{ij}:
						double total = 0;
						for (int j = 0; j < connected.size(); ++j)
						{
							total += CordWeight(v, connected[j]);
						}

						// Now the sum:
						for (int j = 0; j < connected.size(); ++j)
						{
							weightCoefficient = CordWeight(v, connected[j]) / total;
							
							xSum += weightCoefficient * (connected[j]->x - v->x);
							ySum += weightCoefficient * (connected[j]->y - v->y);
							zSum += weightCoefficient * (connected[j]->z - v->z);
						}
						break;
					}

				case Weight::CORD_STATIC:
					{
						// Compute denominator of weight w_{ij}:
						double total = 0;
						for (int j = 0; j < connected.size(); ++j)
						{
							Vert& vi = originalVertices[v->index];
							Vert& vj = originalVertices[connected[j]->index];
							total += CordWeight(&vi, &vj);
						}

						// Now the sum:
						for (int j = 0; j < connected.size(); ++j)
						{
							Vert& vi = originalVertices[v->index];
							Vert& vj = originalVertices[connected[j]->index];
							weightCoefficient = CordWeight(&vi, &vj) / total;
							
							xSum += weightCoefficient * (vj.x - vi.x);
							ySum += weightCoefficient * (vj.y - vi.y);
							zSum += weightCoefficient * (vj.z - vi.z);
						}
							break;
					}

				case Weight::MEAN_CURVATURE_DYNAMIC:
					{
						// Rather than using the list of connected vertices:
						// Bounce around the vertex to get to each of the corners.
						// First get the total:
						double total = 0;
						int k = -1;
						Corner* previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = adjacent->n->angle;
							double phi = adjacent->n->o->angle;
							total += MeanCurvatureWeight(theta, phi);
							previous = adjacent;
						}

						// Now the sum:
						k = -1;
						previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = adjacent->n->angle;
							double phi = adjacent->n->o->angle;
							weightCoefficient = MeanCurvatureWeight(theta, phi) / total;
							
							Vert* w = adjacent->p->v;

							xSum += weightCoefficient * (w->x - v->x);
							ySum += weightCoefficient * (w->y - v->y);
							zSum += weightCoefficient * (w->z - v->z);
							//std::cout << k << " " << c.index << " " << theta << " " << phi << " " << weightCoefficient << std::endl;

							previous = adjacent;
						}
						break;
					}

				case Weight::MEAN_CURVATURE_STATIC:
					{
						// Rather than using the list of connected vertices:
						// Bounce around the vertex to get to each of the corners.
						// First get the total:
						double total = 0;
						int k = -1;
						Corner* previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = adjacent->n->angle;
							double phi = adjacent->n->o->angle;
							total += MeanCurvatureWeight(theta, phi);
							previous = adjacent;
						}

						// Now the sum:
						k = -1;
						previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = adjacent->n->angle;
							double phi = adjacent->n->o->angle;
							weightCoefficient = MeanCurvatureWeight(theta, phi) / total;
							
							Vert* w = adjacent->p->v;

							xSum += weightCoefficient * (originalVertices[w->index].x - originalVertices[v->index].x);
							ySum += weightCoefficient * (originalVertices[w->index].y - originalVertices[v->index].y);
							zSum += weightCoefficient * (originalVertices[w->index].z - originalVertices[v->index].z);
							//std::cout << k << " " << c.index << " " << theta << " " << phi << " " << weightCoefficient << std::endl;

							previous = adjacent;
						}
						break;
					}

				case Weight::MEAN_VALUE_DYNAMIC:
					{
						// Rather than using the list of connected vertices:
						// Bounce around the vertex to get to each of the corners.
						// First get the total:
						double total = 0;
						int k = -1;
						Corner* previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = previous->angle;
							double phi = adjacent->angle;
							total += MeanValueWeight(theta, phi);
							previous = adjacent;
						}

						// Now the sum:
						k = -1;
						previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = previous->angle;
							double phi = adjacent->angle;
							weightCoefficient = MeanValueWeight(theta, phi) / total;
							
							Vert* w = adjacent->p->v;

							xSum += weightCoefficient * (w->x - v->x);
							ySum += weightCoefficient * (w->y - v->y);
							zSum += weightCoefficient * (w->z - v->z);

							previous = adjacent;
						}
						break;
					}

				case Weight::MEAN_VALUE_STATIC:
					{
						// Rather than using the list of connected vertices:
						// Bounce around the vertex to get to each of the corners.
						// First get the total:
						double total = 0;
						int k = -1;
						Corner* previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = previous->angle;
							double phi = adjacent->angle;
							total += MeanValueWeight(theta, phi);
							previous = adjacent;
						}

						// Now the sum:
						k = -1;
						previous = &c;
						while (k != c.index)
						{
							Corner* adjacent = previous->p->o->p;
							k = adjacent->index;
							
							double theta = previous->angle;
							double phi = adjacent->angle;
							weightCoefficient = MeanValueWeight(theta, phi) / total;
							
							Vert* w = adjacent->p->v;

							xSum += weightCoefficient * (originalVertices[w->index].x - originalVertices[v->index].x);
							ySum += weightCoefficient * (originalVertices[w->index].y - originalVertices[v->index].y);
							zSum += weightCoefficient * (originalVertices[w->index].z - originalVertices[v->index].z);
							//std::cout << k << " " << c.index << " " << theta << " " << phi << " " << weightCoefficient << std::endl;

							previous = adjacent;
						}
						break;
					}
			}
			v->x = v->x + dt * xSum;
			v->y = v->y + dt * ySum;
			v->z = v->z + dt * zSum;

			//std::cout << "New coordinates: " << v->x << " " << v->y << " " << v->z << std::endl;
		}
	}
}

void Smoothing::EvaluateMorse0(Polyhedron* p, std::vector<int>& maxima, std::vector<int>& minima, double defaultValue, double dt, int iterations)
{
	// First, go through the vertices and assign initial values.
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		if (isIn(p->vlist[i].index, maxima))
			p->vlist[i].value0 = 1.0;
		else if (isIn(p->vlist[i].index, minima))
			p->vlist[i].value0 = 0.0;
		else
			p->vlist[i].value0 = defaultValue;
	}

	for (int n = 0; n < iterations; ++n)
	{
		// Go through the corners to assign values.
		// To do this, we will need to keep track of which vertices we have seen already.
		// We must never change the values of minima and maxima, so they will be added to the map immediately.
		std::map<int, Vert*> checked;
		for (int i = 0; i < maxima.size(); ++i)
		{
			checked[maxima[i]] = &p->vlist[maxima[i]];
		}
		for (int i = 0; i < minima.size(); ++i)
		{
			checked[minima[i]] = &p->vlist[minima[i]];
		}

		for (Corner& c : p->clist)
		{
			if (!checked.count(c.v->index))
			{
				Vert* v = c.v;
				std::vector<Vert*> connected = c.GetAdjacentVertices();
				checked.insert({ v->index, v });
				
				// Compute denominator of weight w_{ij}:
				double total = 0;
				for (int j = 0; j < connected.size(); ++j)
				{
					total += CordWeight(v, connected[j]);
				}

				// Now the sum:
				double fSum = 0;
				for (int j = 0; j < connected.size(); ++j)
				{
					double weightCoefficient = CordWeight(v, connected[j]) / total;
					fSum += weightCoefficient * (connected[j]->value0 - v->value0);
				}
				v->value0 += dt * fSum;
			}
		}
	}
}

void Smoothing::EvaluateMorse1(Polyhedron* p, std::vector<int>& maxima, std::vector<int>& minima, double defaultValue, double dt, int iterations)
{
	// First, go through the vertices and assign initial values.
	for (int i = 0; i < p->vlist.size(); ++i)
	{
		if (isIn(p->vlist[i].index, maxima))
			p->vlist[i].value1 = 1.0;
		else if (isIn(p->vlist[i].index, minima))
			p->vlist[i].value1 = 0.0;
		else
			p->vlist[i].value1 = defaultValue;
	}

	for (int n = 0; n < iterations; ++n)
	{
		// Go through the corners to assign values.
		// To do this, we will need to keep track of which vertices we have seen already.
		// We must never change the values of minima and maxima, so they will be added to the map immediately.
		std::map<int, Vert*> checked;
		for (int i = 0; i < maxima.size(); ++i)
		{
			checked[maxima[i]] = &p->vlist[maxima[i]];
		}
		for (int i = 0; i < minima.size(); ++i)
		{
			checked[minima[i]] = &p->vlist[minima[i]];
		}

		for (Corner& c : p->clist)
		{
			if (!checked.count(c.v->index))
			{
				Vert* v = c.v;
				std::vector<Vert*> connected = c.GetAdjacentVertices();
				checked.insert({ v->index, v });
				
				// Compute denominator of weight w_{ij}:
				double total = 0;
				for (int j = 0; j < connected.size(); ++j)
				{
					total += CordWeight(v, connected[j]);
				}

				// Now the sum:
				double fSum = 0;
				for (int j = 0; j < connected.size(); ++j)
				{
					double weightCoefficient = CordWeight(v, connected[j]) / total;
					fSum += weightCoefficient * (connected[j]->value1 - v->value1);
				}
				v->value1 += dt * fSum;
			}
		}
	}
}

void Smoothing::SetCriticalPoints0(Polyhedron* p)
{
	// Standard strategy: use corners to loop through vertices.
	// Compare the values of a given vertex to the values of its neighbors.
	std::map<int, Vert*> checked;
	for (Corner& c : p->clist)
	{
		if (!checked.count(c.v->index))
		{
			Vert* v = c.v;
			std::vector<Vert*> connected = c.GetAdjacentVertices();
			checked.insert({ v->index, v });

			bool isMax = true;
			bool isMin = true;

			// Build up a vector of signs: true for f(v) < f(v_i), false for f(v) > f(v_i).
			std::vector<bool> signs;
			signs.reserve(connected.size());
			
			// Loop through all adjacent vertices.
			for (int i = 0; i < connected.size(); ++i)
			{
				// Maximum/minimum check:
				if (v->value0 < connected[i]->value0)
				{
					isMax = false;					
					signs.push_back(true);
				}
				else if (v->value0 > connected[i]->value0)
				{
					isMin = false;				
					signs.push_back(false);
				}
				else
				{
					isMax = false;
					isMin = false;
				}
			}

			// Now determine if this is a saddle point of some kind.
			// Increase the saddle count when a positive (true) changes to negative (false).
			bool flag = signs[0];
			int count = 0;
			for (int i = 1; i < signs.size(); ++i)
			{
				if (flag == false && signs[i] == true)
				//if (flag == true && signs[i] == false)
					++count;

				flag = signs[i];
			}

			// Now assign the vertex to the proper lists.
			if (isMax)
				v->minMax = 1;
			else if (isMin)
				v->minMax = 0;
			else
				if (count >= 2)
					v->saddle = count - 1;
				else
					v->saddle = 0;
		}
	}
}

void Smoothing::SetCriticalPoints1(Polyhedron* p)
{
	// Standard strategy: use corners to loop through vertices.
	// Compare the values of a given vertex to the values of its neighbors.
	std::map<int, Vert*> checked;
	for (Corner& c : p->clist)
	{
		if (!checked.count(c.v->index))
		{
			Vert* v = c.v;
			std::vector<Vert*> connected = c.GetAdjacentVertices();
			checked.insert({ v->index, v });

			bool isMax = true;
			bool isMin = true;

			// Build up a vector of signs: true for f(v) < f(v_i), false for f(v) > f(v_i).
			std::vector<bool> signs;
			signs.reserve(connected.size());
			
			// Loop through all adjacent vertices.
			for (int i = 0; i < connected.size(); ++i)
			{
				// Maximum/minimum check:
				if (v->value1 < connected[i]->value1)
				{
					isMax = false;					
					signs.push_back(true);
				}
				else if (v->value1 > connected[i]->value1)
				{
					isMin = false;				
					signs.push_back(false);
				}
				else
				{
					isMax = false;
					isMin = false;
					signs.push_back(false);
				}
			}

			// Now determine if this is a saddle point of some kind.
			// Increase the saddle count when a positive (true) changes to negative (false).
			bool flag = signs[0];
			int count = 0;
			for (int i = 1; i < signs.size(); ++i)
			{
				if (flag == false && signs[i] == true)
				//if (flag == true && signs[i] == false)
					++count;

				flag = signs[i];
			}

			// Now assign the vertex to the proper lists.
			if (isMax)
				v->minMax = 1;
			else if (isMin)
				v->minMax = 0;
			else
				if (count >= 2)
					v->saddle = count - 1;
				else
					v->saddle = 0;
		}
	}
}

/*
bool Smoothing::isLocalMaximum0(Vert* v, std::vector<Vert*> neighbors)
{
	for (int i = 0; i < neighbors.size(); ++i)
	{
		if (v->value0 < neighbors[i]->value0)
		{
			return false;
		}
	}
	return true;
}
bool Smoothing::isLocalMaximum1(Vert* v, std::vector<Vert*> neighbors)
{
	for (int i = 0; i < neighbors.size(); ++i)
	{
		if (v->value1 < neighbors[i]->value1)
		{
			return false;
		}
	}
	return true;
}
bool Smoothing::isLocalMinimum0(Vert* v, std::vector<Vert*> neighbors)
{
	for (int i = 0; i < neighbors.size(); ++i)
	{
		if (v->value0 > neighbors[i]->value0)
		{
			return false;
		}
	}
	return true;
}
bool Smoothing::isLocalMinimum1(Vert* v, std::vector<Vert*> neighbors)
{
	for (int i = 0; i < neighbors.size(); ++i)
	{
		if (v->value1 > neighbors[i]->value1)
		{
			return false;
		}
	}
	return true;
}
*/

double Smoothing::UniformWeight(int total)
{
	return (double)1.0 / (double)total;
}
double Smoothing::CordWeight(Vert* v, Vert* w)
{
	glm::dvec3 vPosition = glm::dvec3(v->x, v->y, v->z);
	glm::dvec3 wPosition = glm::dvec3(w->x, w->y, w->z);
	return 1.0 / glm::length(vPosition - wPosition);
}
double Smoothing::MeanCurvatureWeight(double theta, double phi)
{
	return 0.5 * ((1.0 / tan(theta)) + (1.0 / tan(phi)));
}
double Smoothing::MeanValueWeight(double theta, double phi)
{
	return 0.5 * (tan(0.5 * theta) + tan(0.5 * phi));
}
bool Smoothing::isIn(int target, std::vector<int> list)
{
	for (int i = 0; i < list.size(); ++i)
	{
		if (target == list[i])
			return true;
	}
	return false;
}
