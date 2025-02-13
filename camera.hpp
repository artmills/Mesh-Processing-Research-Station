#pragma once

#include <iostream>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"

class Camera
{

public:

	Camera();
	~Camera();

	// Step one unit in the following direction.
	void StepDirection(glm::vec3 direction);
	void StepDirection(float x, float y, float z);

	// Step one unit forward or backward.
	void StepFacingDirection(float forward);

	// Step one unit right or left.
	void StepHorizontal(float right);

	// Step one unit up or down.
	void StepUp(float up);

	// Get view matrix.
	glm::mat4 GetViewMatrix();

	// Get the direction vector that the camera is facing.
	glm::vec3 GetDirection();	

	// Get the up vector corresponding to the viewing direction.
	glm::vec3 GetUpVector(glm::mat4& viewMatrix);

	// Invert the camera's pitch.
	void InvertPitch();

	// Debug:
	void Print();

	glm::vec3 position;
	float velocity = 0.5f;
	//float velocity = 5.5f;
	float pitch = 0; // How high or low.
	float yaw = 0; // Left or right.
	float roll = 0; // Tilt.

private:

	// Direction that this vector is facing.
	// This is a vector space in the tangent space of the position in \R^3.
	glm::vec3 directionVector = glm::vec3(0, 0, 0);

	// Keep track of the up-vector.
	// If pitch = 0, this up-vector is along the y-axis (standard OpenGL orientation).
	// This vector gets rotated whenever the pitch and yaw change.
	glm::vec3 upVector = glm::vec3(0, 1, 0);

	// Same as right vector: image of (1, 0, 0) under the view matrix.
	glm::vec3 rightVector = glm::vec3(1, 0, 0);

};
