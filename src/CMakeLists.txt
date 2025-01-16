cmake_minimum_required(VERSION 3.20)

# set policies to new behavior
cmake_policy(SET CMP0120 NEW)

set(SOURCE_FILES
    "KeyCodes.hpp"
"InputManager.cpp"    "InputManager.hpp"
"JSONUtils.cpp"    "JSONUtils.hpp"
"Vertex.hpp"
"Texture.cpp"    		"Texture.hpp"
"ShaderProgram.cpp"    	"ShaderProgram.hpp"
"Singleton.hpp" 								"Singleton.inl"
"ResourceManager.cpp"   "ResourceManager.hpp"	"ResourceManager.inl"
"Model.cpp"    			"Model.hpp"
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})