#include "view.hpp"


View::View() {}
View::~View() {}

glm::dvec3 View::getViewDirection()
{
	return (viewTarget - viewPosition);
}
