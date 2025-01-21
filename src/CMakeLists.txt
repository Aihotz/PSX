cmake_minimum_required(VERSION 3.20)

# set policies to new behavior
cmake_policy(SET CMP0120 NEW)

# set(SOURCE_FILES
    # "LogicComponent.cpp"    "LogicComponent.hpp"
# "LogicSystem.cpp"    "LogicSystem.hpp"
# "HierarchyManager.cpp"    "HierarchyManager.hpp"
# "Rotation.cpp"    "Rotation.hpp"
# "GameObjectManager.cpp"    "GameObjectManager.hpp"
# "TransformationComponent.cpp"    "TransformationComponent.hpp"
# "Transformation.cpp"    "Transformation.hpp"
# "Component.hpp"    "Component.cpp"
# "GameObject.hpp"    "GameObject.cpp"    "GameObject.inl"
# "KeyCodes.hpp"
# "InputManager.cpp"    "InputManager.hpp"
# "JSONUtils.cpp"    "JSONUtils.hpp"
# "Vertex.hpp"
# "Texture.cpp"    		"Texture.hpp"
# "ShaderProgram.cpp"    	"ShaderProgram.hpp"
# "Singleton.hpp" 								"Singleton.inl"
# "ResourceManager.cpp"   "ResourceManager.hpp"	"ResourceManager.inl"
# "Model.cpp"    			"Model.hpp"
# )

file(GLOB_RECURSE SOURCE_FILES
	"./*.cpp"
	"./*.hpp"
	"./*.inl")

list(REMOVE_ITEM SOURCE_FILES "./main.cpp")

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})