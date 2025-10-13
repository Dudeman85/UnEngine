#include <source_location>

#include "UnEngine.h"
#include "Vector.h"
#include "Debug.h"

int main()
{
	debug::logOutputs.push_back({new std::ofstream{"log.txt"}, false});
	debug::verbosity = debug::Verbosity::Info;

	GLFWwindow* window = une::CreateGLWindow(800, 600, "Window");

	une::EngineInit();
	une::Camera cam = une::Camera(800, 600);
	cam.SetPosition({0, 0, 3000});
	cam.perspective = false;
	une::renderer::SetBackgroundColor(une::Color(100, 20, 150));

	une::Texture texture("../../../examples/assets/strawberry.png");
	une::Texture transparentTexture("../../../examples/assets/Transparent.png");
	une::Model model("../../../examples/assets/Achelous.obj");
	une::Primitive square = une::Primitive::Rectangle();
	une::Font font("../../../examples/assets/Coolvetica Rg Cond.otf", 24);
	une::Tilemap tilemap("../../../examples/assets/testMap.tmx");

	ecs::Entity e0 = ecs::NewEntity();
	//ecs::AddComponent(e0, une::SpriteRenderer{.texture = &texture});
	ecs::AddComponent(e0, une::Transform{.position = {-50, 30, -10}, .scale = 150});

	ecs::Entity e1 = ecs::NewEntity();
	//ecs::AddComponent(e1, une::SpriteRenderer{.texture = &transparentTexture, .enabled = true});
	ecs::AddComponent(e1, une::Transform{.position = {0, 0, -20}, .scale = 100});

	ecs::Entity e2 = ecs::NewEntity();
	ecs::AddComponent(e2, une::SpriteRenderer{.texture = &texture});
	//ecs::AddComponent(e2, une::ModelRenderer{.model = &model});
	//ecs::AddComponent(e2, une::PrimitiveRenderer{.primitive = &square, .color = une::Color(250, 50, 250, 100)});
	//ecs::AddComponent(e2, une::TextRenderer{.font = &font, .text = "Helloqp  World!", .color = une::Color(0, 0, 0, 255)});
	ecs::AddComponent(e2, une::TilemapRenderer{.tilemap = &tilemap});
	ecs::AddComponent(e2, une::Transform{.position = {0, 0, 0}, .rotation = {0, 0, 0}, .scale = 1});


	while (!glfwWindowShouldClose(window))
	{
		if (glfwGetKey(window, GLFW_KEY_RIGHT))
		{
			une::TransformSystem::Translate(e2, 2, 0, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT))
		{
			une::TransformSystem::Translate(e2, -2, 0, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_UP))
		{
			une::TransformSystem::Translate(e2, 0, 2, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN))
		{
			une::TransformSystem::Translate(e2, 0, -2, 0);
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
		{
			une::TransformSystem::Translate(e2, 0, 0, 1);
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
		{
			une::TransformSystem::Translate(e2, 0, 0, -1);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_ADD))
		{
			cam.Translate({0, 0, -10});
		}
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT))
		{
			cam.Translate({0, 0, 10});
		}
		if (glfwGetKey(window, GLFW_KEY_W))
		{
			cam.Translate({0, 10, 0});
		}
		if (glfwGetKey(window, GLFW_KEY_A))
		{
			cam.Translate({-10, 0, 0});
		}
		if (glfwGetKey(window, GLFW_KEY_S))
		{
			cam.Translate({0, -10, 0});
		}
		if (glfwGetKey(window, GLFW_KEY_D))
		{
			cam.Translate({10, 0, 0});
		}

		une::Update(&cam);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}
