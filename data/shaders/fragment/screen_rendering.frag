#version 460 core

// framebuffer
out vec4 output_color;

// from the vertex shader
in vec2 fragment_textureCoordinates;

// uniforms
layout(binding = 0) uniform sampler2D textureData;
layout(location = 2) uniform int current_texture; // 0 position, 1 normal, 2 albedo, 3 depth
layout(location = 3) uniform vec2 frustum_distances;

void main()
{
	vec4 texture_sample = texture(textureData, fragment_textureCoordinates);

	switch(current_texture)
	{
		case 3: // depth
		{
			float z = texture_sample.r * 2.0 - 1.0;
			float near = frustum_distances[0];
			float far = frustum_distances[1];
			float value = (2.0 * near) / (far + near - z * (far - near));
			output_color = vec4(vec3(value), 1.0);
			break;
		}
		
		case 0: // position
			output_color = texture_sample;
			break;
		
		case 1: // normal
			output_color = vec4(texture_sample.xyz * 0.5 + 0.5, 1.0);
			break;
			
		case 2: // albedo
		default:
			output_color = texture_sample;
			break;
	}
}