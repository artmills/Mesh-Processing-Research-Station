#include "camera.hpp"


Camera::Camera(){}
Camera::~Camera(){}

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
	position += velocity * GetDirection();
}

glm::vec3 Camera::GetDirection()
{
	return glm::vec3(sin(yaw) * cos(pitch), -sin(pitch), -cos(pitch) * cos(yaw));
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 viewMatrix = glm::rotate(glm::mat4(1), this->pitch, glm::vec3(1, 0, 0));
	viewMatrix = glm::rotate(viewMatrix, this->yaw, glm::vec3(0, 1, 0));
	viewMatrix = glm::translate(viewMatrix, -1.0f * this->position);
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
