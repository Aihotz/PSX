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

// Main code
int main(int, char**)
{
    // Setup SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return -1;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    Uint32      window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    SDL_Window* window       = SDL_CreateWindow("Dear ImGui SDL3+OpenGL3 example", 1280, 720, window_flags);
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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glbinding::Binding::initialize([](const char* name)
        { return reinterpret_cast<glbinding::ProcAddress>(SDL_GL_GetProcAddress(name)); },
        false);

    gl::glEnable(gl::GL_DEBUG_OUTPUT);
    gl::glDebugMessageCallback(DebugCallback, 0);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    if (SDL_GL_LoadLibrary(nullptr) == false)
    {
        printf("Error loading OpenGL library: %s\n", SDL_GetError());
        return -1;
    }

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        gl::glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        gl::glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        gl::glClear(gl::GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}