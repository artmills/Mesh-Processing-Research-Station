#include "mousepicker.hpp"


MousePicker::MousePicker() {}
MousePicker::MousePicker(int displayWidth, int displayHeight, glm::mat4& projectionMatrix)
{
	this->displayWidth = displayWidth;
	this->displayHeight = displayHeight;
	this->projectionMatrix = projectionMatrix;
}
MousePicker::~MousePicker(){}

void MousePicker::UpdateViewMatrix(glm::mat4& viewMatrix)
{
	this->viewMatrix = viewMatrix;
}

glm::vec3 MousePicker::ComputeRay(glm::mat4& transformationMatrix)
{
	float mouseX = 2.0f * mouseCoordinates.x / displayWidth - 1.0f;
	float mouseY = 2.0f * mouseCoordinates.y / displayHeight - 1.0f;

	glm::mat4 inverseVP = glm::inverse(projectionMatrix * viewMatrix);
	glm::vec4 screenPosition = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
	glm::vec4 worldPosition = inverseVP * screenPosition;
	glm::vec3 direction = glm::normalize(glm::vec3(worldPosition));
	return direction;
	/*
	// Viewport -> Normalized Device Coordinates:
	glm::vec2 ndc = getNormalizedDeviceCoordinates(mouseCoordinates.x, mouseCoordinates.y);

	// Normalized Device Coordintes -> Clip Space Coordinates:
	glm::vec4 clipSpace = glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f); // z = -1 to point into screen.

	// Clip Space -> Eye Space:
	glm::vec4 eyeCoordinates = getEyeSpaceCoordinates(clipSpace);

	// Eye Space -> World Space:
	glm::vec4 worldSpace = glm::vec4(getWorldCoordinates(eyeCoordinates), 1.0f);

	return glm::vec3(worldSpace.x, worldSpace.y, worldSpace.z);
	*/
}
glm::vec3 MousePicker::getRay()
{
	//this->ray = ComputeRay();
	return this->ray;
}

void MousePicker::setMouseCoordinates(int x, int y)
{
	mouseCoordinates = glm::vec2(x, y);
}

glm::vec2 MousePicker::getNormalizedDeviceCoordinates(int mouseX, int mouseY)
{
	float x = 2.0f * mouseX / displayWidth - 1;
	float y = 2.0f * mouseY / displayHeight - 1;
	return glm::vec2(x, -y);
}

glm::vec4 MousePicker::getEyeSpaceCoordinates(glm::vec4 clipSpace)
{
	glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
	glm::vec4 eyeCoordinates = inverseProjection * clipSpace;
	return glm::vec4(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
}

glm::vec3 MousePicker::getWorldCoordinates(glm::vec4 eyeCoordinates)
{
	glm::mat4 inverseView = glm::inverse(viewMatrix);
	glm::vec4 rayWorldCoordinates = inverseView * eyeCoordinates;
	glm::vec3 mouseRay = glm::vec3(rayWorldCoordinates.x, rayWorldCoordinates.y, rayWorldCoordinates.z);
	return glm::normalize(mouseRay);
}
