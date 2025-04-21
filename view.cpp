#include "view.hpp"


View::View() {}
View::~View() {}

View::View(glm::dvec3 position, glm::dvec3 target, Polyhedron* poly)
{
	this->viewPosition = position;
	this->viewTarget = target;
	this->p = poly;
}

glm::dvec3 View::getViewDirection()
{
	return (viewTarget - viewPosition);
}
