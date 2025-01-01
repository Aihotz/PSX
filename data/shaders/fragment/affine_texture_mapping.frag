#version 460 core

in vec2 fragTextureCoordinates;

out vec4 color;
in float fragW;

layout(binding = 0) uniform sampler2D textureData;

void main()
{
	color = texture(textureData, fragTextureCoordinates / fragW);
}