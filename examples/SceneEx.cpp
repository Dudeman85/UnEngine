#include "UnEngine.h"
#include "Vector.h"
#include "Serialization.h"
#include "Networking.h"
#include "renderer/UserInterface.h"
#include "debug/GUI.h"
#include "debug/Logging.h"
#include "debug/Primitives.h"

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
	ecs::AddComponent(camera, une::Transform{.position = {0, 0, 100}});
	une::CameraSystem::MakeOrtho(camera, 800, 600);
	une::UICanvas canvas;

	ecs::Entity test = ecs::NewEntity();
	ecs::AddComponent(test, une::SpriteRenderer{.texture = &texture});
	ecs::AddTag(test, "non world entity");
	ecs::AddTag(test, "tag2");

	ecs::Entity player = ecs::NewEntity();
	ecs::AddComponent(player, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(player, une::Transform{{0.000, 0.000, -10.000}, {0.000, 0.000, 0.000}, {10.000, 10.000, 10.000}, {-5.000, -6.500, 0.000}, une::XYZ, 0, {4, 6}});
	ecs::AddComponent(player, une::PolygonCollider{.vertices = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}}});
	ecs::Entity child = ecs::NewEntity();
	ecs::AddComponent(child, une::TextRenderer{.font = &font, .text = "Player", .color = une::Color(12, 150, 60)});
	ecs::AddComponent(child, une::Transform{.position = {-4.5, 8, 0}, .scale = 0.2});
	une::TransformSystem::AddParent(child, player);
	ecs::Entity nestedChild = ecs::NewEntity();
	ecs::AddComponent(nestedChild, une::TextRenderer{.font = &font, .text = "Name:", .color = une::Color(200, 20, 60)});
	ecs::AddComponent(nestedChild, une::Transform{.position = {0, 17, 0}, .scale = 0.5});
	une::TransformSystem::AddParent(nestedChild, child);
	ecs::Entity child2 = ecs::NewEntity();
	ecs::AddComponent(child2, une::Transform{.position = {0, -8, 0}});
	une::Primitive primitive = une::Primitive::Rectangle();
	//ecs::AddComponent(child2, une::PrimitiveRenderer{.primitive = &primitive, .color = une::Color(12, 150, 60)});
	une::TransformSystem::AddParent(child2, player);

	ecs::Entity uiSprite = ecs::NewEntity();
	ecs::AddComponent(uiSprite, une::TextRenderer{.font = &font, .text = "Health: 9000"});
	ecs::AddComponent(uiSprite, une::Transform{.position = {20, -50, 0}, .scale = 2});
	ecs::AddComponent(uiSprite, une::UIElement{.canvas = &canvas, .anchor = {-1, 1}});
	ecs::Entity uiSprite2 = ecs::NewEntity();
	ecs::AddComponent(uiSprite2, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(uiSprite2, une::Transform{.position = {-50, 0, 0}, .scale = 10});
	ecs::AddComponent(uiSprite2, une::UIElement{.canvas = &canvas, .anchor = {1, 0}});

	// Setup Dear ImGui context

	bool show_demo_window = true;

	//Game loop
	while (!window->ShouldClose())
	{
		//Poll events
		une::BeginFrame();

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
		if (glfwGetKey(window->glWindow, GLFW_KEY_0))
		{
			debug::gui::EnableWindow(debug::gui::ImWindow::Demo);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_1))
		{
			debug::gui::EnableWindow(debug::gui::ImWindow::Entities);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_2))
		{
			debug::gui::EnableWindow(debug::gui::ImWindow::Inspector);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_T))
		{
			une::Transform& tf = ecs::GetComponent<une::Transform>(player);
			debug::DrawRectangle({-200, -200, 0}, {-200, 200, 0}, {200, 200, 0}, tf.position, une::Color(255, 0, 0), false);
		}

		une::CameraSystem::MakeOrtho(camera, window->GetSize().x, window->GetSize().y);
		//TODO: fix
		canvas.SetScale(canvas.GetScale());

		//Update engine libraries and render everything
		une::Update();

		//Swap screen buffers and calculate dt
		une::EndFrame();
	}


	une::UnInit();
	return 0;
}
