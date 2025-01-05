#version 460 core

// quad vertex attributes
layout(location = 0) in vec3 vertex_position;
layout(location = 2) in vec2 vertex_textureCoordinates;

// for the fragment shader
out vec2 fragment_textureCoordinates;

// uniforms
layout(location = 0) uniform mat4 model_matrix;
layout(location = 1) uniform float scale;

void main()
{
	mat4 scale_matrix = mat4(
		scale, 0.0, 0.0, 0.0,
		0.0, scale, 0.0, 0.0,
		0.0, 0.0, scale, 0.0,
		0.0, 0.0, 0.0, 1.0);

	gl_Position = model_matrix * scale_matrix * vec4(vertex_position, 1.0);

	fragment_textureCoordinates = vertex_textureCoordinates;
}
