#ifndef SHADERPROGRAM_HPP
#define SHADERPROGRAM_HPP

#include <glbinding/gl/gl.h>
#include <string>

class ShaderProgram
{
		gl::GLuint handle;

		// so that the resource manager can instantiate the class 'Shader'
		friend class ResourceManager;
		class Shader
		{
				gl::GLuint handle;

			public:
				static constexpr char DUMMY_FRAGMENT_SHADER_PATH[] = "DUMMY_FRAGMENT_SHADER";
				static constexpr char DUMMY_VERTEX_SHADER_PATH[]   = "DUMMY_VERTEX_SHADER";

				static constexpr char VERTEX_SHADER_EXTENSION[] = "vert";
				static constexpr char FRAGMENT_SHADER_EXTENSION[] = "frag";

				Shader(const std::string& filename);
				~Shader();

				gl::GLuint GetHandle();

				Shader(const Shader&)			 = delete;
				Shader& operator=(const Shader&) = delete;
		};

		static constexpr char FRAGMENT_SHADER_JSON_KEY[] = "fragment";
		static constexpr char VERTEX_SHADER_JSON_KEY[] = "vertex";

	public:
		ShaderProgram(const std::string& filepath);
		~ShaderProgram();

		void Bind();

		gl::GLuint GetHandle();

		// avoid unintended copying
		ShaderProgram(const ShaderProgram&)			   = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
};

#endif