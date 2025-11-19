#include "UnEngine.h"
#include "Vector.h"
#include "Debug.h"
#include "Serialization.h"

#include <vector>

#include "Networking.h"
#include "renderer/UserInterface.h"

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
	ecs::AddComponent(camera, une::Camera{.viewport = {0, 0, 1, 1}});
	ecs::AddComponent(camera, une::Transform{.position = {0, 0, 3000}});
	une::CameraSystem::MakeOrtho(camera, 800, 600);
	une::UICanvas canvas(800, 600);

	ecs::Entity player = ecs::NewEntity();
	ecs::AddComponent(player, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(player, une::Transform{.position = {0, 0, 0}, .scale = 5});

	ecs::Entity uiSprite = ecs::NewEntity();
	ecs::AddComponent(uiSprite, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(uiSprite, une::Transform{.position = {50, -50, 0}});
	ecs::AddComponent(uiSprite, une::UIElement{.canvas = &canvas, .anchor = {-1, 1}});

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
			une::TransformSystem::Translate(camera, 0, 0, -10);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_KP_SUBTRACT))
		{
			window->SetSize({800, 600});
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
		if (glfwGetKey(window->glWindow, GLFW_KEY_F))
		{
			une::Camera& cam = ecs::GetComponent<une::Camera>(camera);
			cam.viewport.x2--;
			une::CameraSystem::MakeOrtho(camera, cam.viewport.x2, cam.viewport.y2);
			//canvas.SetSize(cam.viewport.x2, cam.viewport.y2);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_G))
		{
			une::Camera& cam = ecs::GetComponent<une::Camera>(camera);
			cam.viewport.y2--;
			une::CameraSystem::MakeOrtho(camera, cam.viewport.x2, cam.viewport.y2);
			//canvas.SetSize(cam.viewport.x2, cam.viewport.y2);
		}

		//Update engine libraries and render everything
		une::Update();
	}

	return 0;
}
