#pragma once

#include <iostream>
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{

public:

	Camera();
	~Camera();

	// Step one unit in the following direction.
	void StepDirection(glm::vec3 direction);
	void StepDirection(float x, float y, float z);

	// Get view matrix.
	glm::mat4 GetViewMatrix();

	// Get the direction vector that the camera is facing.
	glm::vec3 GetDirection();	

	// Invert the camera's pitch.
	void InvertPitch();

	// Debug:
	void Print();

	glm::vec3 position;
	float velocity = 10.0f;
	float pitch = 0; // How high or low.
	float yaw = 0; // Left or right.
	float roll = 0; // Tilt.

private:

	// Direction that this vector is facing.
	// This is a vector space in the tangent space of the position in \R^3.
	glm::vec3 directionVector = glm::vec3(0, 0, 0);

};
