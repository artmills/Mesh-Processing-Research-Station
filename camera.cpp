#include "camera.hpp"


Camera::Camera(){}
Camera::~Camera(){}

void Camera::StepFacingDirection(float forward)
{
	position += forward * velocity * directionVector;
}
void Camera::StepUp(float up)
{
	position += up * velocity * upVector;
}
void Camera::StepHorizontal(float right)
{
	position += right * velocity * rightVector;
}
void Camera::StepDirection(glm::vec3 direction)
{
	position += velocity * direction;
}
void Camera::StepDirection(float x, float y, float z)
{
	/*
	position.x += velocity * x;
	position.y += velocity * y;
	position.z += velocity * z;
	*/
	//position += velocity * GetDirection();
	position += velocity * glm::vec3(x, y, z);
}

glm::vec3 Camera::GetDirection()
{
	return this->directionVector;
}

glm::vec3 Camera::GetUpVector(glm::mat4& viewMatrix)
{
	return this->upVector;
}

/*
glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 viewMatrix = glm::rotate(glm::mat4(1), this->roll, glm::vec3(0, 0, 1));
	viewMatrix = glm::rotate(viewMatrix, this->pitch, glm::vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, this->yaw, glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(viewMatrix, -1.0f * this->position);

	// While we're here, set the direction vector.
	//this->directionVector = glm::vec3(sin(yaw) * cos(pitch), -sin(pitch), -cos(pitch) * cos(yaw));
	this->directionVector = -glm::vec3(
			-sin(yaw) * cos(roll) - cos(yaw) * sin(pitch) * sin(roll),
			sin(yaw) * sin(roll) - cos(yaw) * sin(pitch) * cos(roll),
			cos(yaw) * cos(pitch)
			);

	// Also while we're here, let's set the up vector to be the viewMatrix * (0, 1, 0, 1)... which is the second column.
	this->upVector = glm::vec3(glm::column(viewMatrix, 1));

	// And also might as well get the right vector.
	//this->rightVector = glm::vec3(glm::column(viewMatrix, 0));
	this->rightVector = glm::cross(this->directionVector, upVector);

	return viewMatrix;
}
*/

glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 viewMatrix = glm::rotate(glm::mat4(1), this->pitch, glm::vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, this->yaw, glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(viewMatrix, -1.0f * this->position);

	// While we're here, set the direction vector.
	this->directionVector = glm::vec3(sin(yaw) * cos(pitch), -sin(pitch), -cos(pitch) * cos(yaw));

	// Also while we're here, let's set the up vector to be the viewMatrix * (0, 1, 0, 1)... which is the second column.
	this->upVector = glm::vec3(glm::column(viewMatrix, 1));

	// And also might as well get the right vector.
	//this->rightVector = glm::vec3(glm::column(viewMatrix, 0));
	this->rightVector = glm::cross(this->directionVector, upVector);

	return viewMatrix;
}

void Camera::Print()
{
	std::cout << "Camera: " << position.x << " " << position.y << " ";
	std::cout << position.z << " - " << pitch << " " << yaw << std::endl;
}

void Camera::InvertPitch()
{
	pitch += -glm::pi<float>() / 2.0f;
}
