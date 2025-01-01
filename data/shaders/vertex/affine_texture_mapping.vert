#version 460 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 textureCoordinates;

out vec2 fragTextureCoordinates;
out float fragW;

layout(location = 0) uniform mat4 viewproj;
layout(location = 1) uniform mat4 model;
layout(location = 2) uniform bool usingAffineTextureMapping;

void main()
{
	gl_Position = viewproj * model * vec4(position, 1.0f);

	fragW = 1.0f;
	if(usingAffineTextureMapping)
	{
		fragW = gl_Position.w;
	}

	fragTextureCoordinates = textureCoordinates * fragW;
}
