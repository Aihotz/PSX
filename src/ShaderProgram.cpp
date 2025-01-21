#include "ShaderProgram.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include "JSONUtils.hpp"
#include "ResourceManager.hpp"

using namespace gl;

ShaderProgram::ShaderProgram(const std::string& filepath)
{
	std::string vertex_shader_path;
	std::string fragment_shader_path;

	if (std::filesystem::exists(std::filesystem::path { filepath }))
	{
		nlohmann::json shader_program_json = LoadJSONFromFile(filepath);

		vertex_shader_path	 = shader_program_json[VERTEX_SHADER_JSON_KEY];
		fragment_shader_path = shader_program_json[FRAGMENT_SHADER_JSON_KEY];
	}
	else
	{
		vertex_shader_path	 = Shader::DUMMY_VERTEX_SHADER_PATH;
		fragment_shader_path = Shader::DUMMY_FRAGMENT_SHADER_PATH;
	}

	Resource<Shader> vertex_shader { vertex_shader_path };
	Resource<Shader> fragment_shader { fragment_shader_path };

	handle = glCreateProgram();
	glAttachShader(handle, vertex_shader->GetHandle());
	glAttachShader(handle, fragment_shader->GetHandle());
	glLinkProgram(handle);

	// print linking errors
	GLint success;
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (static_cast<GLboolean>(success) == GL_FALSE)
	{
		int length = 0;
		gl::glGetProgramiv(handle, gl::GL_INFO_LOG_LENGTH, &length);

		std::string log;

		if (length > 0)
		{
			log.resize(length);

			int written = 0;
			gl::glGetProgramInfoLog(handle, length, &written, log.data());
		}

		std::cerr << "Could not link shader program \"" + filepath + "\": " << log << std::endl;
	}
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(handle);
}

void ShaderProgram::Bind()
{
	glUseProgram(handle);
}

GLuint ShaderProgram::GetHandle()
{
	return handle;
}

std::string GetDummyShaderCode(gl::GLenum shader_type)
{
	if (shader_type == GL_FRAGMENT_SHADER)
	{
		return R"(
		#version 460
		
		out vec4 out_color;

		void main()
		{
			out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		)";
	}
	else // if shader_type == GL_VERTEX_SHADER
	{
		return R"(
		#version 460
		
		layout(location = 0) in vec3 position;

		layout(location = 0) uniform mat4 viewproj;
		layout(location = 1) uniform mat4 model;

		void main()
		{
			gl_Position = viewproj * model * vec4(position, 1.0f);
		}
		)";
	}
}

ShaderProgram::Shader::Shader(const std::string& filepath)
{
	std::string shader_code;
	GLenum		shader_type = GL_VERTEX_SHADER;

	if (filepath == DUMMY_VERTEX_SHADER_PATH)
	{
		shader_type = GL_VERTEX_SHADER;
		shader_code = GetDummyShaderCode(shader_type);
	}
	else if (filepath == DUMMY_FRAGMENT_SHADER_PATH)
	{
		shader_type = GL_FRAGMENT_SHADER;
		shader_code = GetDummyShaderCode(shader_type);
	}
	else
	{
		std::filesystem::path shader_path { filepath };

		// get shader type
		std::string file_extension = shader_path.extension().string().substr(1);
		if (file_extension == VERTEX_SHADER_EXTENSION)
		{
			shader_type = GL_VERTEX_SHADER;
		}
		else if (file_extension == FRAGMENT_SHADER_EXTENSION)
		{
			shader_type = GL_FRAGMENT_SHADER;
		}

		// get shader code
		if (std::filesystem::exists(std::filesystem::path { shader_path }))
		{
			std::ifstream shader_file(filepath);
			if (shader_file.is_open() == false)
			{
				std::cerr << "Could not open file \"" << filepath << "\"." << std::endl;
				shader_code = GetDummyShaderCode(shader_type);
			}

			std::stringstream file_buffer;
			file_buffer << shader_file.rdbuf();
			shader_code = file_buffer.str();

			shader_file.close();
		}
		else
		{
			shader_code = GetDummyShaderCode(shader_type);
		}
	}

	// compile shader
	handle = glCreateShader(shader_type);

	const char* shader_source_cstr = shader_code.c_str();
	glShaderSource(handle, 1, &shader_source_cstr, nullptr);
	glCompileShader(handle);

	// print shader compilation errors
	GLint success;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (static_cast<GLboolean>(success) == GL_FALSE)
	{
		int length = 0;
		gl::glGetShaderiv(handle, gl::GL_INFO_LOG_LENGTH, &length);

		std::string log;

		if (length > 0)
		{
			log.resize(length);

			int written = 0;
			gl::glGetShaderInfoLog(handle, length, &written, log.data());
		}

		std::cerr << "Could not compile shader \"" + filepath + "\": " << log << std::endl;
	}
}

GLuint ShaderProgram::Shader::GetHandle()
{
	return handle;
}

ShaderProgram::Shader::~Shader()
{
	glDeleteShader(handle);
}