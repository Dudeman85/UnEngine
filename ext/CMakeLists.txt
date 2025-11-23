# Build UnEngine Dependencies

#GLM
add_subdirectory("glm")

#GLFW
add_subdirectory("glfw")

#Freetype
add_subdirectory("freetype")

#ENet
add_subdirectory("enet")

#TmxLite
set(TMXLITE_STATIC_LIB ON CACHE BOOL "")
add_subdirectory("tmxlite/tmxlite")

#Assimp
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
add_subdirectory("assimp")

# MiniUPnPc
set(UPNPC_BUILD_SAMPLE ON)
set(UPNPC_BUILD_SHARED OFF)
set(UPNPC_BUILD_TESTS OFF)
set(UPNPC_NO_INSTALL ON)
set(NO_GETADDRINFO ON)
add_subdirectory("miniupnp/miniupnpc")

# ImGui
include_directories("imgui")
file(GLOB IMGUI_SOURCES "./imgui/*.cpp")
file(GLOB IMGUI_HEADERS "./imgui/*.h")
set(IMGUI_BACKEND 
	"./imgui/backends/imgui_impl_opengl3.cpp"
	"./imgui/backends/imgui_impl_opengl3.h"
	"./imgui/backends/imgui_impl_glfw.cpp"
	"./imgui/backends/imgui_impl_glfw.h"
)
add_library(imgui ${IMGUI_SOURCES} ${IMGUI_HEADERS} ${IMGUI_BACKEND})
target_link_libraries(imgui glfw)