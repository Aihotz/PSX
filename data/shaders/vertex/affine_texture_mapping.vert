#version 460 core

// vertex attributes
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;
layout(location = 2) in vec2 vertex_textureCoordinates;
layout(location = 3) in vec3 vertex_normal;

// for the fragment shader
out vec3 fragment_view_position;
out vec3 fragment_color;
out vec2 fragment_textureCoordinates;
out vec3 fragment_normal;
out float fragment_W;

// uniforms
layout(location = 0) uniform mat4 projection_matrix;
layout(location = 1) uniform mat4 view_matrix;
layout(location = 2) uniform mat4 model_matrix;

layout(location = 3) uniform bool usingAffineTextureMapping;

void main()
{		
	vec4 view_position = view_matrix * model_matrix * vec4(vertex_position, 1.0);
	fragment_view_position = view_position.xyz;
	gl_Position = projection_matrix * vec4(floor(view_position.xyz), 1.0);

	fragment_color = vertex_color;

	fragment_W = 1.0;
	if(usingAffineTextureMapping)
	{
		fragment_W = gl_Position.w;
	}
	fragment_textureCoordinates = vertex_textureCoordinates * fragment_W;
	
	mat4 normal_matrix = transpose(inverse(view_matrix * model_matrix));
	fragment_normal = normalize(normal_matrix * vec4(vertex_normal, 0.0)).xyz;
}