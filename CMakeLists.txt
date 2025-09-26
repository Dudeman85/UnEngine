cmake_minimum_required(VERSION 3.20)
project(UnEngine)

option(UNENGINE_BUILD_EXAMPLES "Build example scene" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TMXLITE_STATIC_LIB ON CACHE BOOL "")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)

include_directories("ext")
include_directories("ext/enet/include")
include_directories("include")

add_subdirectory("ext/enet")
add_subdirectory("ext/freetype")
add_subdirectory("ext/assimp")
add_subdirectory("ext/tmxlite/tmxlite")
add_subdirectory("ext/glfw")
add_subdirectory("ext/glm")

set(GLAD_GL "ext/glad/gl.h" "ext/glad/gl.c")

# engine -library
file(GLOB_RECURSE ENGINE_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "./include/*.h")
add_library(UnEngine ${ENGINE_SOURCE_FILES} ${ENGINE_HEADER_FILES} ${GLAD_GL})

target_link_libraries(UnEngine glfw glm assimp tmxlite freetype enet)
	
target_include_directories(UnEngine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(UNENGINE_BUILD_EXAMPLES)
	add_subdirectory("examples")
endif()
