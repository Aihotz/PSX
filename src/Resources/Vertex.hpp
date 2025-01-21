#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

struct Vertex
{
		enum AttributeLocations
		{
			POSITION,
			COLOR,
			TEXTURE_COORDINATES,
			NORMAL
		};

		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 textureCoordinates;
		glm::vec3 normal;
};

#endif