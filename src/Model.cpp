#include "Model.hpp"

#include <filesystem>
#include <iostream>

#include <Vertex.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace gl;

void LoadModel(const std::string& filepath, std::vector<Vertex>& vertices, std::vector<int>& indices)
{
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = "./"; // path to material files
	reader_config.vertex_color	  = true;

	reader_config.triangulate = true;
	constexpr size_t fv		  = 3;

	// needs to import earcut into the project!
	//reader_config.triangulation_method = "earcut";

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(filepath, reader_config))
	{
		if (!reader.Error().empty())
		{
			std::cerr << "TinyObjReader: " << reader.Error();
		}

		return;
	}

	if (!reader.Warning().empty())
	{
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	const tinyobj::attrib_t&				attrib	  = reader.GetAttrib();
	const std::vector<tinyobj::shape_t>&	shapes	  = reader.GetShapes();
	const std::vector<tinyobj::material_t>& materials = reader.GetMaterials();

	size_t indexCount = 0;
	for (const tinyobj::shape_t& shape : shapes)
	{
		indexCount += shape.mesh.indices.size();
	}

	vertices.reserve(attrib.vertices.size() / fv);
	indices.reserve(indexCount);

	std::map<std::tuple<int, int, int>, int> unique_vertices;

	// loop over shapes
	size_t shapes_size = shapes.size();
	for (size_t s = 0; s < shapes_size; s++)
	{
		// loop over faces(polygon)
		size_t index_offset		 = 0;
		size_t num_face_vertices = shapes[s].mesh.num_face_vertices.size();
		for (size_t f = 0; f < num_face_vertices; f++)
		{
			// loop over vertices in the face
			//size_t fv = static_cast<size_t>(shapes[s].mesh.num_face_vertices[f]);
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				std::tuple<int, int, int> key = std::make_tuple(idx.vertex_index, idx.normal_index, idx.texcoord_index);

				if (unique_vertices.find(key) != unique_vertices.end())
				{
					indices.push_back(unique_vertices[key]);
					continue;
				}

				size_t current_index = unique_vertices.size();
				unique_vertices[key] = current_index;
				indices.push_back(unique_vertices[key]);

				Vertex vertex;

				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				vertex.position = glm::vec3 { vx, vy, vz };

				// check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

					vertex.normal = glm::vec3 { nx, ny, nz };
				}

				// check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

					vertex.textureCoordinates = glm::vec2 { tx, ty };
				}

				// vertex colors
				// there should always be color, even if default
				tinyobj::real_t red	  = attrib.colors[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t green = attrib.colors[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t blue  = attrib.colors[3 * size_t(idx.vertex_index) + 2];

				vertex.color = glm::vec3 { red, green, blue };

				vertices.push_back(vertex);
			}

			index_offset += fv;

			// per-face material
			//shapes[s].mesh.material_ids[f];
		}
	}
}

void CreateCube(std::vector<Vertex>& vertices, std::vector<int>& indices)
{
	constexpr glm::vec3 white { 1.0f, 1.0f, 1.0f };

	constexpr glm::vec3 positions[] {
		{ -0.5f, 0.5f,  0.5f }, // 0 - left top near
		{ -0.5f, -0.5f, 0.5f }, // 1 - left bottom near
		{ 0.5f,	-0.5f, 0.5f }, // 2 - right bottom near
		{ 0.5f,	0.5f,  0.5f }, // 3 - right top near
		{ 0.5f,	0.5f,  -0.5f  }, // 4 - right top far
		{ 0.5f,	-0.5f, -0.5f	}, // 5 - right bottom far
		{ -0.5f, -0.5f, -0.5f	 }, // 6 - left bottom far
		{ -0.5f, 0.5f,  -0.5f	}, // 7 - left top far
	};

	constexpr glm::vec2 uvs[] {
		{ 0.0f, 1.0f }, // 0 - top-left
		{ 0.0f, 0.0f }, // 1 - bottom-left
		{ 1.0f, 0.0f }, // 2 - bottom-right
		{ 1.0f, 1.0f }, // 3 - top-right
	};

	constexpr glm::vec3 normals[] {
		{ 0.0f,	1.0f,  0.0f  }, // 0 - top
		{ 0.0f,	-1.0f, 0.0f	}, // 1 - bottom
		{ -1.0f, 0.0f,  0.0f	}, // 2 - left
		{ 1.0f,	0.0f,  0.0f  }, // 3 - right
		{ 0.0f,	0.0f,  1.0f  }, // 4 - inward
		{ 0.0f,	0.0f,  -1.0f }, // 5 - outward
	};

	// position, uv, normal
	constexpr std::tuple<int, int, int> index_tuples[] {
		{ 0, 0, 5 },
		 { 1, 1, 5 },
		  { 2, 2, 5 },
		   { 0, 0, 5 },
		{ 2, 2, 5 },
		 { 3, 3, 5 },

		{ 3, 0, 3 },
		 { 2, 1, 3 },
		  { 5, 2, 3 },
		   { 3, 0, 3 },
		{ 5, 2, 3 },
		 { 4, 3, 3 },

		{ 4, 0, 4 },
		 { 5, 1, 4 },
		  { 6, 2, 4 },
		   { 4, 0, 4 },
		{ 6, 2, 4 },
		 { 7, 3, 4 },

		{ 7, 0, 2 },
		 { 6, 1, 2 },
		  { 1, 2, 2 },
		   { 7, 0, 2 },
		{ 1, 2, 2 },
		 { 0, 3, 2 },

		{ 7, 0, 0 },
		 { 0, 1, 0 },
		  { 3, 2, 0 },
		   { 7, 0, 0 },
		{ 3, 2, 0 },
		 { 4, 3, 0 },

		{ 1, 0, 1 },
		 { 6, 1, 1 },
		  { 5, 2, 1 },
		   { 1, 0, 1 },
		{ 5, 2, 1 },
		 { 2, 3, 1 },
	};

	std::map<std::tuple<int, int, int>, int> unique_indices;

	for (const std::tuple<int, int, int>& index_tuple : index_tuples)
	{
		if (unique_indices.find(index_tuple) != unique_indices.end())
		{
			indices.push_back(unique_indices[index_tuple]);
			continue;
		}

		size_t current_size			= unique_indices.size();
		unique_indices[index_tuple] = current_size;
		indices.push_back(unique_indices[index_tuple]);

		vertices.push_back(Vertex {
			positions[std::get<0>(index_tuple)],
			white,
			uvs[std::get<1>(index_tuple)],
			normals[std::get<2>(index_tuple)],
		});
	}
}

Model::Model(const std::string& filepath)
{
	std::vector<Vertex> vertices;
	std::vector<int>	indices;

	if (std::filesystem::exists(std::filesystem::path { filepath }))
	{
		LoadModel(filepath, vertices, indices);
	}
	else
	{
		CreateCube(vertices, indices);
	}

	vertexCount = vertices.size();
	indexCount	= indices.size();

	// generate handlers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// VAO
	glBindVertexArray(vao);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(Vertex::POSITION);
	glEnableVertexAttribArray(Vertex::NORMAL);
	glEnableVertexAttribArray(Vertex::TEXTURE_COORDINATES);
	glEnableVertexAttribArray(Vertex::COLOR);

	glVertexAttribPointer(
		Vertex::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
	glVertexAttribPointer(
		Vertex::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glVertexAttribPointer(
		Vertex::TEXTURE_COORDINATES,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		reinterpret_cast<void*>(offsetof(Vertex, textureCoordinates)));
	glVertexAttribPointer(
		Vertex::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::Render()
{
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

Model::~Model()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}