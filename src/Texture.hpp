#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <string>

class Texture
{
		gl::GLuint handle;
		
		static constexpr glm::ivec2 DUMMY_TEXTURE_SIZE = glm::ivec2 { 16, 16 };

	public:
		struct TextureInfo
		{
				void*	   data;
				glm::ivec2 size;

				gl::GLint  internal_format;
				gl::GLenum format;
				gl::GLenum data_type;

				std::vector<std::pair<gl::GLenum, gl::GLint>> parameters;
		};

		static constexpr char RENDER_TARGET[] = "EMPTY_TEXTURE";

		Texture(const std::string& filepath);

		~Texture();

		void Bind();
		void UploadTextureData(const TextureInfo& info);

		Texture(const Texture&)			   = delete;
		Texture& operator=(const Texture&) = delete;
};

#endif