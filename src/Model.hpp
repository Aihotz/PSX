#ifndef MODEL_HPP
#define MODEL_HPP

#include <glbinding/gl/gl.h>
#include <string>

class Model
{
		gl::GLuint vao;
		gl::GLuint vbo;
		gl::GLuint ebo;

		int vertexCount;
		int indexCount;

	public:
		static constexpr char QUAD_PRIMITIVE[] = "QUAD_PRIMITIVE";
		static constexpr char CUBE_PRIMITIVE[] = "CUBE_PRIMITIVE";

		Model(const std::string& filepath);
		~Model();

		void Render();

		// avoid unintended copying
		Model(const Model&)			   = delete;
		Model& operator=(const Model&) = delete;
};

#endif