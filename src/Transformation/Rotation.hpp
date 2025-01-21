#ifndef ROTATION_HPP
#define ROTATION_HPP

#include <glm/glm.hpp>

struct Rotation
{
		static constexpr glm::vec3 VECTOR_UP	  = glm::vec3 { 0.0f, 1.0f, 0.0f };
		static constexpr glm::vec3 VECTOR_FORWARD = glm::vec3 { 0.0f, 0.0f, -1.0f };
		static constexpr glm::vec3 VECTOR_RIGHT	  = glm::vec3 { 1.0f, 0.0f, 0.0f };

		Rotation();
		Rotation(glm::vec3 forward, glm::vec3 up = glm::vec3 { 0.f, 1.f, 0.f });
		Rotation(glm::vec3 forward, glm::vec3 up, glm::vec3 right);

		void RotateAxis(float angle, glm::vec3 axis);

		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;
};

#endif