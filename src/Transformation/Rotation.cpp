#include "Rotation.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>

Rotation::Rotation()
	: forward { 0.0f, 0.0f, -1.0f }, up { 0.0f, 1.0f, 0.0f }, right {1.0f, 0.0f, 0.0f}
{

}

Rotation::Rotation(glm::vec3 forward, glm::vec3 up)
	: forward { forward }, up { up }
{
	right = glm::normalize(glm::cross(forward, up));
}

Rotation::Rotation(glm::vec3 forward, glm::vec3 up, glm::vec3 right)
	: forward { forward }, up { up }, right {right}
{

}

void Rotation::RotateAxis(float angle, glm::vec3 axis)
{
	glm::mat4 rotation_mtx = glm::rotateNormalizedAxis(glm::mat4 { 1.0f }, angle, axis);

	forward = glm::normalize(glm::vec3(rotation_mtx * glm::vec4 { forward, 0.0f }));
	up		 = glm::normalize(glm::vec3(rotation_mtx * glm::vec4 { up, 0.0f }));
	right	 = glm::normalize(glm::vec3(rotation_mtx * glm::vec4 { right, 0.0f }));
}
