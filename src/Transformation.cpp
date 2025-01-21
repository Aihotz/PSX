#include "Transformation.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

Transformation::Transformation(glm::vec3 position, glm::vec3 scale, Rotation rotation)
	: position { position }, scale { scale }, rotation { rotation }
{
}

Transformation ExtractTransformation(const glm::mat4& matrix)
{
	// will be filled
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;

	// ignored
	glm::vec3 skew;
	glm::vec4 perspective;

	// get the information from the input matrix
	glm::decompose(matrix, scale, rotation, translation, skew, perspective);
	glm::highp_quat inverse_rotation = glm::inverse(glm::conjugate(rotation));

	// fill the resulting transformation with it
	Transformation result;
	result.position			= translation;
	result.scale			= scale;
	result.rotation.forward = glm::normalize(glm::rotate(inverse_rotation, glm::vec4 { 0.0f, 0.0f, -1.0f, 0.0f }));
	result.rotation.up		= glm::normalize(glm::rotate(inverse_rotation, glm::vec4 { 0.0f, 1.0f, 0.0f, 0.0f }));
	result.rotation.right	= glm::normalize(glm::rotate(inverse_rotation, glm::vec4 { 1.0f, 0.0f, 0.0f, 0.0f }));

	return result;
}

glm::mat4 Transformation::GetMatrix() const
{
	glm::mat4 lookat	= glm::lookAt(position, position + rotation.forward, rotation.up);
	glm::mat4 world_mtx = glm::inverse(lookat) * glm::scale(glm::mat4 { 1.0f }, scale);

	return world_mtx;
}

Transformation Transformation::Concatenate(const Transformation& child_local) const
{
	glm::mat4 child_model_to_world = GetMatrix() * child_local.GetMatrix();

	return ExtractTransformation(child_model_to_world);
}

Transformation Transformation::operator*(const Transformation& child_local) const
{
	return Concatenate(child_local);
}

Transformation Transformation::InverseConcatenate(const Transformation& child_world) const
{
	glm::mat4 child_world_to_model = glm::inverse(GetMatrix()) * child_world.GetMatrix();

	return ExtractTransformation(child_world_to_model);
}

void Transformation::LookAt(glm::vec3 target)
{
	// do not deal with zero vectors
	if (glm::all(glm::epsilonEqual(target, position, glm::epsilon<float>())))
		return;

	rotation.forward = glm::normalize(target - position);
	rotation.right	 = glm::normalize(glm::cross(rotation.forward, glm::vec3 { 0.0f, 1.0f, 0.0f }));
	rotation.up		 = glm::normalize(glm::cross(rotation.right, rotation.forward));
}