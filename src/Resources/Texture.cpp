#include "Texture.hpp"

#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace gl;

uint8_t* GenerateDummyTexture(int width, int height)
{
	int		 texture_size = width * height;
	uint8_t* texture_data = new uint8_t[texture_size * 4];

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			uint8_t color = (x + y) % 2 == 0 ? 255 : 0;

			texture_data[(y * width + x) * 4 + 0] = color;
			texture_data[(y * width + x) * 4 + 1] = color;
			texture_data[(y * width + x) * 4 + 2] = color;
			texture_data[(y * width + x) * 4 + 3] = color;
		}
	}

	return texture_data;
}

Texture::Texture(const std::string& filepath)
{
	TextureInfo texture_info;
	uint8_t*	texture_data;

	bool loaded_from_stb = false;

	if (filepath == RENDER_TARGET)
	{
		// expects UploadTextureData() at some point
		return;
	} 
	else if (std::filesystem::exists(std::filesystem::path { filepath }))
	{
		int comp;
		texture_data = stbi_load(filepath.c_str(), &texture_info.size.x, &texture_info.size.y, &comp, 4);

		if (texture_data)
		{
			loaded_from_stb = true;
		}
		else
		{
			texture_info.size = DUMMY_TEXTURE_SIZE;
			texture_data	  = GenerateDummyTexture(texture_info.size.x, texture_info.size.y);
		}
	}
	else
	{
		texture_info.size = DUMMY_TEXTURE_SIZE;
		texture_data	  = GenerateDummyTexture(texture_info.size.x, texture_info.size.y);
	}

	texture_info.data			 = texture_data;
	texture_info.format			 = GL_RGBA;
	texture_info.internal_format = static_cast<GLint>(GL_RGBA);
	texture_info.data_type		 = GL_UNSIGNED_BYTE;

	texture_info.parameters.push_back(
		std::pair<GLenum, GLint> { GL_TEXTURE_WRAP_S, static_cast<GLint>(GL_CLAMP_TO_EDGE) });
	texture_info.parameters.push_back(
		std::pair<GLenum, GLint> { GL_TEXTURE_WRAP_T, static_cast<GLint>(GL_CLAMP_TO_EDGE) });
	texture_info.parameters.push_back(
		std::pair<GLenum, GLint> { GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST) });
	texture_info.parameters.push_back(
		std::pair<GLenum, GLint> { GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST) });

	UploadTextureData(texture_info);

	// we could do this calling free()
	// but just in case the stbi implementation changes
	if (loaded_from_stb)
	{
		stbi_image_free(texture_data);
	}
	else
	{
		delete[] texture_data;
	}
}

void Texture::UploadTextureData(const Texture::TextureInfo& info)
{
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);

	// set texture parameters
	for (const std::pair<GLenum, GLint> param : info.parameters)
	{
		glTexParameteri(GL_TEXTURE_2D, param.first, param.second);
	}

	// send texture data
	glTexImage2D(
		GL_TEXTURE_2D, 0, info.internal_format, info.size.x, info.size.y, 0, info.format, info.data_type, info.data);

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &handle);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, handle);
}