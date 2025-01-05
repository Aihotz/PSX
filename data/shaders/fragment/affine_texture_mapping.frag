#version 460 core

// output to the G-Buffer
layout (location = 0) out vec4 output_position;
layout (location = 1) out vec4 output_normal;
layout (location = 2) out vec4 output_color;

// from the vertex shader
in vec3 fragment_view_position;
in vec3 fragment_color;
in vec2 fragment_textureCoordinates;
in vec3 fragment_normal;
in float fragment_W;

// uniforms
layout(binding = 0) uniform sampler2D textureData;

void main()
{
	output_position = vec4(fragment_view_position, 1.0);

	vec3 normal = normalize(fragment_normal);
	output_normal = vec4(normal, 1.0);
	
	output_color = vec4(fragment_color, 1.0) * texture(textureData, fragment_textureCoordinates / fragment_W);
}