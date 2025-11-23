#include "UnEngine.h"
#include "Vector.h"
#include "Debug.h"
#include "Serialization.h"

#include <vector>

#include "Networking.h"
#include "renderer/UserInterface.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

enum class MessageType : int8_t { AddPlayer, RemovePlayer, ConnectionReceived, PositionInfo, AllPositionInfo };

int main()
{
	//Set logging level and outputs
	debug::logOutputs.push_back({new std::ofstream{"log.txt"}, false});
	debug::logOutputs[0].second = true;
	debug::verbosity = debug::Verbosity::Info;

	//Setup une scene
	une::Window* window = une::CreateMainWindow(800, 600, "Window");

	une::EngineInit();
	une::renderer::SetBackgroundColor(une::Color(32, 32, 32));

	std::string assets = UNE_EXAMPLE_PROJECT_LOCATION "/assets/";
	une::Texture texture(assets + "strawberry.png");
	une::Texture transparentTexture(assets + "Transparent.png");
	une::Model model(assets + "Achelous.obj");
	une::Font font(assets + "Coolvetica Rg Cond.otf", 64);
	une::Tilemap tilemap(assets + "testMap.tmx");
	une::Primitive square = une::Primitive::Rectangle();

	//Make the camera and UI canvas
	ecs::Entity camera = ecs::NewEntity();
	ecs::AddComponent(camera, une::Camera{});
	ecs::AddComponent(camera, une::Transform{});
	une::CameraSystem::MakeOrtho(camera, 800, 600);
	une::UICanvas canvas;

	ecs::Entity player = ecs::NewEntity();
	ecs::AddComponent(player, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(player, une::Transform{.position = {0, 0, 0}, .scale = 10, .pivot = {-5, -6.5, 0}});

	ecs::Entity uiSprite = ecs::NewEntity();
	ecs::AddComponent(uiSprite, une::TextRenderer{.font = &font, .text = "Health: 9000"});
	ecs::AddComponent(uiSprite, une::Transform{.position = {20, -50, 0}, .scale = 2});
	ecs::AddComponent(uiSprite, une::UIElement{.canvas = &canvas, .anchor = {-1, 1}});
	ecs::Entity uiSprite2 = ecs::NewEntity();
	ecs::AddComponent(uiSprite2, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(uiSprite2, une::Transform{.position = {-50, 0, 0}, .scale = 10});
	ecs::AddComponent(uiSprite2, une::UIElement{.canvas = &canvas, .anchor = {1, 0}});

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window->glWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	//Game loop
	while (!window->ShouldClose())
	{
		if (glfwGetKey(window->glWindow, GLFW_KEY_RIGHT))
		{
			une::TransformSystem::Translate(player, 2, 0, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_LEFT))
		{
			une::TransformSystem::Translate(player, -2, 0, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_UP))
		{
			une::TransformSystem::Translate(player, 0, 2, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_DOWN))
		{
			une::TransformSystem::Translate(player, 0, -2, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_KP_ADD))
		{
			une::TransformSystem::Rotate(player, 0, 0, 2);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_KP_SUBTRACT))
		{
			une::TransformSystem::Rotate(player, 0, 0, -2);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_W))
		{
			une::TransformSystem::Translate(camera, 0, 10, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_A))
		{
			une::TransformSystem::Translate(camera, -10, 0, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_S))
		{
			une::TransformSystem::Translate(camera, 0, -10, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_D))
		{
			une::TransformSystem::Translate(camera, 10, 0, 0);
		}

		une::CameraSystem::MakeOrtho(camera, window->GetSize().x, window->GetSize().y);
		//TODO move to window resize func
		canvas.SetScale(canvas.GetScale());

		//Update engine libraries and render everything
		une::Update();
	}

	return 0;
}
