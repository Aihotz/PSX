#define USING_IMGUI

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#ifdef _WIN32
#	pragma warning(disable : 4251) // To avoid Windows' warnings when we include <glbinding/Binding.h>
#endif
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
#include <imgui.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include <stb_image.h>

#include <iostream>

#include <chrono>

namespace
{
	static constexpr int WINDOW_WIDTH  = 1920;
	static constexpr int WINDOW_HEIGHT = 1080;
} //namespace

void DebugCallback(
	gl::GLenum		  source,
	gl::GLenum		  type,
	gl::GLuint		  id,
	gl::GLenum		  severity,
	gl::GLsizei		  length,
	const gl::GLchar* message,
	const void*		  userParam)
{
	if (severity == gl::GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	std::string source_str {};
	std::string type_str {};
	std::string severity_str {};

	switch (source)
	{
		case gl::GL_DEBUG_SOURCE_API:			  source_str = "API"; break;
		case gl::GL_DEBUG_SOURCE_WINDOW_SYSTEM:	  source_str = "WINDOW SYSTEM"; break;
		case gl::GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "SHADER COMPILER"; break;
		case gl::GL_DEBUG_SOURCE_THIRD_PARTY:	  source_str = "THIRD PARTY"; break;
		case gl::GL_DEBUG_SOURCE_APPLICATION:	  source_str = "APPLICATION"; break;
		case gl::GL_DEBUG_SOURCE_OTHER:			  source_str = "UNKNOWN"; break;

		default: source_str = "UNKNOWN"; break;
	}

	switch (type)
	{
		case gl::GL_DEBUG_TYPE_ERROR:				type_str = "ERROR"; break;
		case gl::GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "DEPRECATED BEHAVIOR"; break;
		case gl::GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	type_str = "UNDEFINED BEHAVIOR"; break;
		case gl::GL_DEBUG_TYPE_PORTABILITY:			type_str = "PORTABILITY"; break;
		case gl::GL_DEBUG_TYPE_PERFORMANCE:			type_str = "PERFORMANCE"; break;
		case gl::GL_DEBUG_TYPE_OTHER:				type_str = "OTHER"; break;
		case gl::GL_DEBUG_TYPE_MARKER:				type_str = "MARKER"; break;

		default: type_str = "UNKNOWN"; break;
	}

	switch (severity)
	{
		case gl::GL_DEBUG_SEVERITY_HIGH:		 severity_str = "HIGH"; break;
		case gl::GL_DEBUG_SEVERITY_MEDIUM:		 severity_str = "MEDIUM"; break;
		case gl::GL_DEBUG_SEVERITY_LOW:			 severity_str = "LOW"; break;
		case gl::GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "NOTIFICATION"; break;

		default: severity_str = "UNKNOWN"; break;
	}

	printf(
		"%d: %s of %s severity, raised from %s: %s\n",
		id,
		type_str.c_str(),
		severity_str.c_str(),
		source_str.c_str(),
		message);
}

void Initialize();
void Update(float delta);
void Render();
void Shutdown();

int main(int, char**)
{
	// setup SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 460";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

	// create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	Uint32		window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
	SDL_Window* window		 = SDL_CreateWindow("PSX", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
	if (window == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (gl_context == nullptr)
	{
		printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
		return -1;
	}

	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync
	SDL_ShowWindow(window);

	// setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	// setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	glbinding::Binding::initialize(
		[](const char* name) { return reinterpret_cast<glbinding::ProcAddress>(SDL_GL_GetProcAddress(name)); }, false);

	gl::glEnable(gl::GL_DEBUG_OUTPUT);
	gl::glDebugMessageCallback(DebugCallback, 0);

	if (SDL_GL_LoadLibrary(nullptr) == false)
	{
		printf("Error loading OpenGL library: %s\n", SDL_GetError());
		return -1;
	}

	stbi_set_flip_vertically_on_load(static_cast<bool>(true));

	Initialize();

	// initialize delta time
	// dont use auto keyword
	std::chrono::high_resolution_clock::time_point last_time = std::chrono::high_resolution_clock::now();

	bool running = true;
	while (running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
				running = false;
		}
		if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
		{
			SDL_Delay(10);
			continue;
		}

		// compute delta time
		std::chrono::high_resolution_clock::time_point current_time = std::chrono::high_resolution_clock::now();
		float delta = std::chrono::duration<float>(current_time - last_time).count();
		last_time	= current_time;

		Update(delta);

		// start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		Render();

		gl::glPushDebugGroup(gl::GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ImGui");
		// rendering
		ImGui::Render();
		gl::glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		gl::glPopDebugGroup();

		SDL_GL_SwapWindow(window);
	}

	Shutdown();

	// cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DestroyContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ResourceManager.hpp"
#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"

using namespace gl;

struct Camera
{
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;
};

struct Object
{
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		glm::vec3 m_scale;

		// mutable because bind operations are not const
		mutable Resource<Model>	  model;
		mutable Resource<Texture> texture;

		void Render() const
		{
			if (model.IsValid() == false)
				return;

			glm::mat4 model_mtx = glm::translate(glm::mat4(1.0f), m_position)
								* glm::rotate(glm::mat4(1.0f), m_rotation.x, glm::vec3(1, 0, 0))
								* glm::rotate(glm::mat4(1.0f), m_rotation.y, glm::vec3(0, 1, 0))
								* glm::rotate(glm::mat4(1.0f), m_rotation.z, glm::vec3(0, 0, 1))
								* glm::scale(glm::mat4(1.0f), m_scale);

			// send uniforms to shader
			glUniformMatrix4fv(2, 1, GL_FALSE, &model_mtx[0][0]);

			// send texture
			if (texture.IsValid())
			{
				glActiveTexture(GL_TEXTURE0);
				texture->Bind();
			}

			// render
			model->Render();

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
		};
};

namespace
{
	Camera				camera;
	std::vector<Object> objects;
	float				rotation_duration;

	Resource<ShaderProgram> geometry_shader;
	Resource<ShaderProgram> screen_shader;

	Resource<Model> quad;

	bool usingAffineTextureMapping;
	bool showingAllTextures;

	// progressive: 320x240
	// interlaced:  640x480
	bool				 usingInterlacedResolution;
	constexpr glm::ivec2 framebufferResolution[2] {
		glm::ivec2 { 320, 240 },
		 glm::ivec2 { 640, 480 }
	};

	// deferred shading
	GLuint GBuffer[2];

	// position: x y z | ?
	// normal:   x y z | ?
	// albedo:   x y z | ?
	GLuint gPosition[2], gNormal[2], gAlbedo[2];

	// not in PSX, but will probably use it here
	GLuint gDepth[2];
} // namespace

void GenerateGBuffers()
{
	for (int index = 0; index < 2; index++)
	{
		glGenFramebuffers(1, &GBuffer[index]);
		glBindFramebuffer(GL_FRAMEBUFFER, GBuffer[index]);

		// position attachment
		glGenTextures(1, &gPosition[index]);
		glBindTexture(GL_TEXTURE_2D, gPosition[index]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA16F,
			framebufferResolution[index].x,
			framebufferResolution[index].y,
			0,
			GL_RGBA,
			GL_HALF_FLOAT,
			nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition[index], 0);

		// normal attachment
		glGenTextures(1, &gNormal[index]);
		glBindTexture(GL_TEXTURE_2D, gNormal[index]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA16F,
			framebufferResolution[index].x,
			framebufferResolution[index].y,
			0,
			GL_RGBA,
			GL_HALF_FLOAT,
			nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal[index], 0);

		// albedo attachment
		glGenTextures(1, &gAlbedo[index]);
		glBindTexture(GL_TEXTURE_2D, gAlbedo[index]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGB5_A1,
			framebufferResolution[index].x,
			framebufferResolution[index].y,
			0,
			GL_RGBA,
			GL_UNSIGNED_SHORT_5_5_5_1,
			nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo[index], 0);

		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(sizeof(attachments) / sizeof(attachments[0]), attachments);

		// depth attachment
		glGenTextures(1, &gDepth[index]);
		glBindTexture(GL_TEXTURE_2D, gDepth[index]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_DEPTH_COMPONENT16,
			framebufferResolution[index].x,
			framebufferResolution[index].y,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth[index], 0);

		// check for completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Error: Framebuffer is not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Initialize()
{
	usingAffineTextureMapping = true;
	usingInterlacedResolution = false;
	showingAllTextures		  = false;

	rotation_duration = 5.0f;

	// vsync
	SDL_GL_SetSwapInterval(1);

	// enable backface removal
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// enable depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthRange(0, 1);

	// enable scissor test
	glEnable(GL_SCISSOR_TEST);

	// polygon mode: fill the triangles by default
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	objects.push_back(Object {});

	camera.m_viewMatrix
		= glm::lookAt(glm::vec3 { 0.0f, 75.0f, 125.0f }, glm::vec3 { 0.0f, 0.0f, 0.0f }, glm::vec3 { 0.0f, 1.0f, 0.0f });
	camera.m_projectionMatrix
		= glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 1000.0f);

	//Resource<Model>	  model { "data/meshes/maxwell.obj" };
	Resource<Texture> texture { "data/images/maxwell.jpg" };
	Resource<Model>	  model { Model::CUBE_PRIMITIVE };
	//Resource<Texture> texture { "data/images/maxwell.jpg" };

	geometry_shader = Resource<ShaderProgram> { "data/shaders/affine_texture_mapping.json" };
	screen_shader	= Resource<ShaderProgram> { "data/shaders/screen_rendering.json" };

	quad = Resource<Model> { Model::QUAD_PRIMITIVE };

	for (Object& obj : objects)
	{
		obj.model	= model;
		obj.texture = texture;

		//obj.m_scale = glm::vec3 { 0.2f };
		obj.m_scale = glm::vec3 { 50.0f };
	}

	GenerateGBuffers();
}

void Update(float delta)
{
	// rotate all objects around the vertical axis at a rate of 360 degrees per 5 seconds
	for (Object& object : objects)
	{
		object.m_rotation.y += -glm::radians(360.0f / rotation_duration) * delta;

		// also rotate slightly around the right axis, but slower
		//object.m_rotation.x += glm::radians(360.0f / 20.0f) * delta;
	}
}

void RenderGUI()
{
	if (ImGui::Begin("Features"))
	{
		ImGui::Checkbox("Affine texture mapping", &usingAffineTextureMapping);

		ImGui::DragFloat("Rotation duration", &rotation_duration);
	}
	ImGui::End();
}

void Render()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Geometry pass");

	geometry_shader->Bind();

	glUniform1i(3, static_cast<int>(usingAffineTextureMapping));

	// bind non-default framebuffer
	constexpr int mode = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, GBuffer[mode]);

	// clear
	glViewport(0, 0, framebufferResolution[mode].x, framebufferResolution[mode].y);
	glScissor(0, 0, framebufferResolution[mode].x, framebufferResolution[mode].y);

	// position
	glm::vec4 position_clear { glm::vec3 { std::numeric_limits<float>::max() }, 0 };
	glClearBufferfv(GL_COLOR, 0, &position_clear[0]);

	// normal
	glm::vec4 normal_clear { 0.0f, 0.0f, -1.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 1, &normal_clear[0]);

	// albedo
	glm::vec4 color_clear { glm::vec3 { 0.7f } , 1.0f};
	glClearBufferfv(GL_COLOR, 2, &color_clear[0]);

	// depth
	glClear(GL_DEPTH_BUFFER_BIT);

	// render objects
	glUniformMatrix4fv(0, 1, GL_FALSE, &camera.m_projectionMatrix[0][0]);
	glUniformMatrix4fv(1, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);

	for (const Object& object : objects)
		object.Render();

	glPopDebugGroup();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Render to screen");

	screen_shader->Bind();

	// render regular framebuffer (screen)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// clear
	static constexpr glm::ivec2 window_size = glm::ivec2 { WINDOW_WIDTH, WINDOW_HEIGHT };
	glViewport(0, 0, window_size.x, window_size.y);
	glScissor(0, 0, window_size.x, window_size.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int scale_factor = 1;
	while (framebufferResolution[mode].x * (scale_factor + 1) <= window_size.x
		   && framebufferResolution[mode].y * (scale_factor + 1) <= window_size.y)
	{
		scale_factor++;
	}

	glm::ivec2 uniform_scaled_size = scale_factor * framebufferResolution[mode];

	glViewport(
		(window_size.x - uniform_scaled_size.x) / 2,
		(window_size.y - uniform_scaled_size.y) / 2,
		uniform_scaled_size.x,
		uniform_scaled_size.y);
	glScissor(
		(window_size.x - uniform_scaled_size.x) / 2,
		(window_size.y - uniform_scaled_size.y) / 2,
		uniform_scaled_size.x,
		uniform_scaled_size.y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind textures
	glActiveTexture(GL_TEXTURE0);

	glm::mat4 quad_model_matrix;
	if (showingAllTextures)
	{
		// scale for the textures
		glUniform1f(1, 1.0f);

		// near and far
		glUniform2f(3, 0.1f, 100.0f);

		GLuint	  attachments[]	  = { gPosition[mode], gNormal[mode], gAlbedo[mode], gDepth[mode] };
		glm::vec2 displacements[] = {
			{ -0.5f, -0.5f },
			  { 0.5f,  0.5f  },
			  { -0.5f, 0.5f	},
			   { 0.5f,  -0.5f }
		};

		for (int i = 0; i < 4; i++)
		{
			// bind the current attachment
			glBindTexture(GL_TEXTURE_2D, attachments[i]);
			glUniform1i(2, i);

			// matrix to render in each of the screen quadrants
			quad_model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(displacements[i], 0.0f));
			glUniformMatrix4fv(0, 1, GL_FALSE, &quad_model_matrix[0][0]);
			quad->Render();
		}
	}
	else
	{
		// scale for the texture
		glUniform1f(1, 2.0f);

		// using the color texture
		glBindTexture(GL_TEXTURE_2D, gAlbedo[mode]);
		glUniform1i(2, 2);

		// identity matrix (in addition to the 2x scale)
		quad_model_matrix = glm::identity<glm::mat4>();
		glUniformMatrix4fv(0, 1, GL_FALSE, &quad_model_matrix[0][0]);
		quad->Render();
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	glUseProgram(0);

	glPopDebugGroup();

	RenderGUI();
}

void Shutdown()
{
	ResourceManager::GetInstance().DeleteResources();
}