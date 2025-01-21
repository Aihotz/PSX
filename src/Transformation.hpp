#ifndef TRANSFORMATION_HPP
#define TRANSFORMATION_HPP

#include <glm/glm.hpp>
#include "Rotation.hpp"

struct Transformation
{
		Transformation(
			glm::vec3 position = glm::vec3 { 0.0f, 0.0f, 0.0f },
			glm::vec3 scale	   = glm::vec3 { 1.0f, 1.0f, 1.0f },
			Rotation  rotation = Rotation {});

		glm::mat4 GetMatrix() const;

		Transformation Concatenate(const Transformation& child_local) const;
		Transformation operator*(const Transformation& child_local) const;
		Transformation InverseConcatenate(const Transformation& child_world) const;

		void LookAt(glm::vec3 target);

		glm::vec3 position;
		glm::vec3 scale;
		Rotation  rotation;
};

#endif