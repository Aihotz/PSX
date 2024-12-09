cmake_minimum_required(VERSION 3.20)

# Set policies to NEW behavior
cmake_policy(SET CMP0120 NEW)

set(SOURCE_FILES
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})