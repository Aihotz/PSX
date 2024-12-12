#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#ifdef _WIN32
#    pragma warning(disable : 4251) // To avoid Windows' warnings when we include <glbinding/Binding.h>
#endif
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
#include <imgui.h>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>

#include <iostream>

#include <chrono>

namespace
{
    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
}

void DebugCallback(
    gl::GLenum        source,
    gl::GLenum        type,
    gl::GLuint        id,
    gl::GLenum        severity,
    gl::GLsizei       length,
    const gl::GLchar* message,
    const void*       userParam)
{
    if (severity == gl::GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    std::string source_str {};
    std::string type_str {};
    std::string severity_str {};

    switch (source)
    {
        case gl::GL_DEBUG_SOURCE_API: source_str = "API"; break;
        case gl::GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "WINDOW SYSTEM"; break;
        case gl::GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "SHADER COMPILER"; break;
        case gl::GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "THIRD PARTY"; break;
        case gl::GL_DEBUG_SOURCE_APPLICATION: source_str = "APPLICATION"; break;
        case gl::GL_DEBUG_SOURCE_OTHER: source_str = "UNKNOWN"; break;

        default: source_str = "UNKNOWN"; break;
    }

    switch (type)
    {
        case gl::GL_DEBUG_TYPE_ERROR: type_str = "ERROR"; break;
        case gl::GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "DEPRECATED BEHAVIOR"; break;
        case gl::GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: type_str = "UNDEFINED BEHAVIOR"; break;
        case gl::GL_DEBUG_TYPE_PORTABILITY: type_str = "PORTABILITY"; break;
        case gl::GL_DEBUG_TYPE_PERFORMANCE: type_str = "PERFORMANCE"; break;
        case gl::GL_DEBUG_TYPE_OTHER: type_str = "OTHER"; break;
        case gl::GL_DEBUG_TYPE_MARKER: type_str = "MARKER"; break;

        default: type_str = "UNKNOWN"; break;
    }

    switch (severity)
    {
        case gl::GL_DEBUG_SEVERITY_HIGH: severity_str = "HIGH"; break;
        case gl::GL_DEBUG_SEVERITY_MEDIUM: severity_str = "MEDIUM"; break;
        case gl::GL_DEBUG_SEVERITY_LOW: severity_str = "LOW"; break;
        case gl::GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "NOTIFICATION"; break;

        default: severity_str = "UNKNOWN"; break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n", id, type_str.c_str(), severity_str.c_str(), source_str.c_str(), message);
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    Uint32      window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    SDL_Window* window       = SDL_CreateWindow("PSX", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
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

    glbinding::Binding::initialize([](const char* name)
        { return reinterpret_cast<glbinding::ProcAddress>(SDL_GL_GetProcAddress(name)); },
        false);

    gl::glEnable(gl::GL_DEBUG_OUTPUT);
    gl::glDebugMessageCallback(DebugCallback, 0);

    if (SDL_GL_LoadLibrary(nullptr) == false)
    {
        printf("Error loading OpenGL library: %s\n", SDL_GetError());
        return -1;
    }

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
        float                                          delta        = std::chrono::duration<float>(current_time - last_time).count();
        last_time                                                   = current_time;

        Update(delta);

        // start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        gl::glClear(gl::GL_COLOR_BUFFER_BIT);

        Render();

        // rendering
        ImGui::Render();
        gl::glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

using namespace gl;

struct Camera
{
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;
};

struct Vertex
{
    enum AttributeLocation
    {
        POSITION            = 0,
        NORMAL              = 1,
        TEXTURE_COORDINATES = 2,
        COLOR               = 3
    };

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoordinates;
    glm::vec3 color;
};

struct Object
{
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;

    GLuint m_vao;
    GLuint m_vbo;

    std::vector<Vertex> m_vertices;

    GLuint m_texture;

    void Initialize()
    {
        // initialize the object
        // generate VAO and VBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindVertexArray(m_vao);

        // upload mesh
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

        // populate VBO
        glEnableVertexAttribArray(Vertex::POSITION);
        glEnableVertexAttribArray(Vertex::NORMAL);
        glEnableVertexAttribArray(Vertex::TEXTURE_COORDINATES);
        glEnableVertexAttribArray(Vertex::COLOR);

        glVertexAttribPointer(Vertex::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
        glVertexAttribPointer(Vertex::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glVertexAttribPointer(Vertex::TEXTURE_COORDINATES, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, textureCoordinates)));
        glVertexAttribPointer(Vertex::COLOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // initialize texture
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // send texture data
        // create a 16x16 black and white checkerboard pattern
        uint8_t data[16 * 16 * 4];
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                uint8_t color              = (i + j) % 2 == 0 ? 255 : 0;
                data[(i * 16 + j) * 4 + 0] = color;
                data[(i * 16 + j) * 4 + 1] = color;
                data[(i * 16 + j) * 4 + 2] = color;
                data[(i * 16 + j) * 4 + 3] = 255;
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    };

    void Render(const glm::mat4& view_projection, GLuint shader_program) const
    {
        // render the object
        glBindVertexArray(m_vao);

        glUseProgram(shader_program);

        glm::mat4 model =
            glm::translate(glm::mat4(1.0f), m_position) *
            glm::rotate(glm::mat4(1.0f), m_rotation.x, glm::vec3(1, 0, 0)) *
            glm::rotate(glm::mat4(1.0f), m_rotation.y, glm::vec3(0, 1, 0)) *
            glm::rotate(glm::mat4(1.0f), m_rotation.z, glm::vec3(0, 0, 1)) *
            glm::scale(glm::mat4(1.0f), m_scale);

        // send uniforms to shader: viewproj, model and texture data
        glUniformMatrix4fv(0, 1, GL_FALSE, &view_projection[0][0]);
        glUniformMatrix4fv(1, 1, GL_FALSE, &model[0][0]);

        // send texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());

        glBindVertexArray(0);
    };

    void Shutdown()
    {
        // shutdown the object
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    };
};

namespace
{
    Camera              camera;
    std::vector<Object> objects;

    GLuint shader;
} // namespace

void Initialize()
{
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

    // no colors or normals needed for the cube, only the positions and uvs
    static const std::vector<Vertex> cube_primitive {
        // Front face
        // First triangle
        { { -0.5f, -0.5f, 0.5f }, {}, { 0.0f, 0.0f }, {} },
        { { 0.5f, -0.5f, 0.5f }, {}, { 1.0f, 0.0f }, {} },
        { { 0.5f, 0.5f, 0.5f }, {}, { 1.0f, 1.0f }, {} },
        // Second triangle
        { { 0.5f, 0.5f, 0.5f }, {}, { 1.0f, 1.0f }, {} },
        { { -0.5f, 0.5f, 0.5f }, {}, { 0.0f, 1.0f }, {} },
        { { -0.5f, -0.5f, 0.5f }, {}, { 0.0f, 0.0f }, {} },

        // Back face
        // First triangle
        { { 0.5f, -0.5f, -0.5f }, {}, { 0.0f, 0.0f }, {} },
        { { -0.5f, -0.5f, -0.5f }, {}, { 1.0f, 0.0f }, {} },
        { { -0.5f, 0.5f, -0.5f }, {}, { 1.0f, 1.0f }, {} },
        // Second triangle
        { { -0.5f, 0.5f, -0.5f }, {}, { 1.0f, 1.0f }, {} },
        { { 0.5f, 0.5f, -0.5f }, {}, { 0.0f, 1.0f }, {} },
        { { 0.5f, -0.5f, -0.5f }, {}, { 0.0f, 0.0f }, {} },

        // Left face
        // First triangle
        { { -0.5f, -0.5f, -0.5f }, {}, { 0.0f, 0.0f }, {} },
        { { -0.5f, -0.5f, 0.5f }, {}, { 1.0f, 0.0f }, {} },
        { { -0.5f, 0.5f, 0.5f }, {}, { 1.0f, 1.0f }, {} },
        // Second triangle
        { { -0.5f, 0.5f, 0.5f }, {}, { 1.0f, 1.0f }, {} },
        { { -0.5f, 0.5f, -0.5f }, {}, { 0.0f, 1.0f }, {} },
        { { -0.5f, -0.5f, -0.5f }, {}, { 0.0f, 0.0f }, {} },

        // Right face
        // First triangle
        { { 0.5f, -0.5f, 0.5f }, {}, { 0.0f, 0.0f }, {} },
        { { 0.5f, -0.5f, -0.5f }, {}, { 1.0f, 0.0f }, {} },
        { { 0.5f, 0.5f, -0.5f }, {}, { 1.0f, 1.0f }, {} },
        // Second triangle
        { { 0.5f, 0.5f, -0.5f }, {}, { 1.0f, 1.0f }, {} },
        { { 0.5f, 0.5f, 0.5f }, {}, { 0.0f, 1.0f }, {} },
        { { 0.5f, -0.5f, 0.5f }, {}, { 0.0f, 0.0f }, {} },

        // Top face
        // First triangle
        { { -0.5f, 0.5f, 0.5f }, {}, { 0.0f, 0.0f }, {} },
        { { 0.5f, 0.5f, 0.5f }, {}, { 1.0f, 0.0f }, {} },
        { { 0.5f, 0.5f, -0.5f }, {}, { 1.0f, 1.0f }, {} },
        // Second triangle
        { { 0.5f, 0.5f, -0.5f }, {}, { 1.0f, 1.0f }, {} },
        { { -0.5f, 0.5f, -0.5f }, {}, { 0.0f, 1.0f }, {} },
        { { -0.5f, 0.5f, 0.5f }, {}, { 0.0f, 0.0f }, {} },

        // Bottom face
        // First triangle
        { { -0.5f, -0.5f, -0.5f }, {}, { 0.0f, 0.0f }, {} },
        { { 0.5f, -0.5f, -0.5f }, {}, { 1.0f, 0.0f }, {} },
        { { 0.5f, -0.5f, 0.5f }, {}, { 1.0f, 1.0f }, {} },
        // Second triangle
        { { 0.5f, -0.5f, 0.5f }, {}, { 1.0f, 1.0f }, {} },
        { { -0.5f, -0.5f, 0.5f }, {}, { 0.0f, 1.0f }, {} },
        { { -0.5f, -0.5f, -0.5f }, {}, { 0.0f, 0.0f }, {} }
    };

    objects.push_back(
        Object {
            .m_position { 0.f, 0.f, 0.f },
            .m_rotation { 0.f, 0.f, 0.f },
            .m_scale { 1.0f, 1.0f, 1.0f },

            .m_vertices = cube_primitive,
        });

    camera.m_viewMatrix       = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera.m_projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    for (Object& object : objects)
        object.Initialize();

    // create shader
    const char* vertex_shader_source = R"(
		#version 460 core

        layout(location = 0) in vec3 position;
		layout(location = 2) in vec2 textureCoordinates;

		out vec2 fragTextureCoordinates;

		layout(location = 0) uniform mat4 viewproj;
		layout(location = 1) uniform mat4 model;

		void main()
		{
			gl_Position = viewproj * model * vec4(position, 1.0f);
			fragTextureCoordinates = textureCoordinates;
		}
	)";

    const char* fragment_shader_source = R"(
		#version 460 core

		in vec2 fragTextureCoordinates;

		out vec4 color;

		layout(binding = 0) uniform sampler2D textureData;

		void main()
		{
			color = texture(textureData, fragTextureCoordinates);
		}
	)";

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    // print shader compilation errors
    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertex_shader, 512, nullptr, infoLog);
        std::cerr << "Error: Shader compilation failed: " << infoLog << std::endl;
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    // print shader compilation errors
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragment_shader, 512, nullptr, infoLog);
        std::cerr << "Error: Shader compilation failed: " << infoLog << std::endl;
    }

    shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);

    // print linking errors
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error: Shader linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Update(float delta)
{
    // rotate all objects around the vertical axis at a rate of 360 degrees per 5 seconds
    for (Object& object : objects)
    {
        object.m_rotation.y += glm::radians(360.0f / 5.0f) * delta;

        // also rotate slightly around the right axis, but slower
        object.m_rotation.x += glm::radians(360.0f / 20.0f) * delta;
    }
}

void Render()
{
    static constexpr glm::ivec2 window_size = glm::ivec2 { WINDOW_WIDTH, WINDOW_HEIGHT };

    glViewport(0, 0, window_size.x, window_size.y);
    glScissor(0, 0, window_size.x, window_size.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view_projection = camera.m_projectionMatrix * camera.m_viewMatrix;

    for (const Object& object : objects)
        object.Render(view_projection, shader);
}

void Shutdown()
{
    for (Object& object : objects)
        object.Shutdown();
}