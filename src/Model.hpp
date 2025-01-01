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

    Model(const std::string& filepath);
	~Model();

    void Render();

    // avoid unintended copying
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
};

#endif