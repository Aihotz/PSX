cmake_minimum_required(VERSION 3.20)

# Set policies to NEW behavior
cmake_policy(SET CMP0120 NEW)
cmake_policy(SET CMP0117 NEW)

# Only generate Debug and Release configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

project(PSX)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)
add_subdirectory(external/glbinding EXCLUDE_FROM_ALL)
add_subdirectory(external/JSON EXCLUDE_FROM_ALL)
add_subdirectory(external/FreeType EXCLUDE_FROM_ALL)

# Include directories
include_directories(src)
include_directories(external/imgui)
include_directories(external/glm)
include_directories(external/stb)
include_directories(external/tinyobj)

set(IMGUI_SOURCE_FILES
    external/imgui/imgui.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
    external/imgui/imgui_demo.cpp
)

add_library(imguiLib STATIC ${IMGUI_SOURCE_FILES})
target_link_libraries(imguiLib PRIVATE SDL3::SDL3) # Required since we are using the SDL3 backend

set(DEPENDENCY_LIBRARIES
    SDL3::SDL3
    glbinding
    imguiLib
	freetype
	nlohmann_json::nlohmann_json
)

add_executable(${PROJECT_NAME} src/main.cpp)

# WIN32 FileDialog
target_link_libraries(${PROJECT_NAME} PRIVATE Shlwapi)
target_link_libraries(${PROJECT_NAME} PRIVATE Propsys)

# Copy all dynamic libraries to build directory (just for Windows since it has issues).
# Windows lacks "rpath" which allows for paths embedded into the executable
if (WIN32)
  # Define the actual targets you want to copy
  set(COPY_LIBRARIES
      SDL3::SDL3
      glbinding
	  freetype
  )

  foreach(lib ${COPY_LIBRARIES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ${lib} to the build directory."
    )
  endforeach()
endif()

# Actual project
add_subdirectory(src EXCLUDE_FROM_ALL) # This is used to add all source files other than main.cpp
target_link_libraries(${PROJECT_NAME} PRIVATE ${DEPENDENCY_LIBRARIES})

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/../data
	${CMAKE_CURRENT_BINARY_DIR}/data
)

add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/../.clang-format
    ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copying .clang-format to the build directory."
)