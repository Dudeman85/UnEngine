cmake_minimum_required(VERSION 3.20)
project(UnEngine)

option(UNENGINE_BUILD_EXAMPLES "Build example scenes" ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_compile_definitions(NOMINMAX)

# Add engine source and header files
include_directories("include")
include_directories("ext")

#Add GLAD to engine lib
include_directories("ext/glad")
set(GLAD_GL "ext/glad/gl.h" "ext/glad/gl.c")

# UnEngine library
file(GLOB_RECURSE ENGINE_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER_FILES "./include/*.h")
add_library(UnEngine ${ENGINE_SOURCE_FILES} ${ENGINE_HEADER_FILES} ${GLAD_GL})

# Add dependency libraries
add_subdirectory("ext")
include_directories("ext/enet/include")
include_directories("ext/imgui")
target_link_libraries(UnEngine glfw glm assimp tmxlite freetype enet libminiupnpc-static imgui)

target_include_directories(UnEngine PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

if(UNENGINE_BUILD_EXAMPLES)
	add_subdirectory("examples")
endif()
