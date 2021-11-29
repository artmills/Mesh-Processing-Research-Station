#pragma once

#include "view.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

/** Cast a ray from the camera into the scene at the point where the mouse is clicked. **/
class MousePicker
{

public:

	MousePicker();
	MousePicker(int displayWidth, int displayHeight, glm::mat4& projectionMatrix);
	~MousePicker();

	// Update the view matrix, which will change as the camera moves.
	void UpdateViewMatrix(glm::mat4& viewMatrix);

	// Compute the ray.
	glm::vec3 ComputeRay(glm::mat4& transformationMatrix);

	// Get the ray.
	glm::vec3 getRay();

	// Set the mouse coordinates.
	void setMouseCoordinates(int x, int y);

private:

	glm::vec3 ray;
	glm::vec2 mouseCoordinates;

	int displayWidth;
	int displayHeight;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	glm::vec2 getNormalizedDeviceCoordinates(int mouseX, int mouseY);
	glm::vec4 getEyeSpaceCoordinates(glm::vec4 clipSpace);
	glm::vec3 getWorldCoordinates(glm::vec4 eyeCoordinates);

};
