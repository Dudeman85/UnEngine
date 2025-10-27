#include "UnEngine.h"
#include "Vector.h"
#include "Debug.h"
#include "Serialization.h"

#include <vector>

#include "Networking.h"

struct Foo
{
	const char name[4] = "Foo";
};

int main()
{
	char* data = une::Serialize(une::Vector3(0, 1, 2)).first;
	std::pair<une::Vector3, size_t> v = une::Deserialize<une::Vector3>(data);

	une::Packet packet;
	packet.Write(une::Vector3(1, 2, 3));
	packet.Write(5.0);
	packet.Write(5.f);
	packet.Write(std::string("Hello World!"));
	char bytes[4] = {0x45, 0x70, 0x69, 0x63};
	packet.WriteBytes(bytes, 4);
	packet.Write(Foo());

	une::Vector3 v3 = packet.Read<une::Vector3>().first;
	auto d = packet.Read<double>().first;
	auto f = packet.Read<float>().first;
	auto s = packet.Read<std::string>().first;
	std::vector<char> b = packet.ReadBytes(4);
	auto F = packet.Read<Foo>().first;

	debug::logOutputs.push_back({new std::ofstream{"log.txt"}, false});
	debug::verbosity = debug::Verbosity::Info;

	une::Window* window = une::CreateMainWindow(800, 600, "Window");

	une::EngineInit();
	une::Camera cam = une::Camera(800, 600);
	cam.SetPosition({0, 0, 3000});
	cam.perspective = false;
	une::renderer::SetBackgroundColor(une::Color(32, 32, 32));

	std::string assets = "../../../examples/assets/";
	une::Texture texture(assets + "strawberry.png");
	une::Texture transparentTexture(assets + "Transparent.png");
	une::Model model(assets + "Achelous.obj");
	une::Font font(assets + "Coolvetica Rg Cond.otf", 24);
	une::Tilemap tilemap(assets + "testMap.tmx");
	une::Primitive square = une::Primitive::Rectangle();

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
	//ecs::AddComponent(e2, une::TilemapRenderer{.tilemap = &tilemap});
	ecs::AddComponent(e2, une::Transform{.position = {0, 0, 0}, .rotation = {0, 0, 0}, .scale = 1});

	while (!window->ShouldClose())
	{
		if (glfwGetKey(window->glWindow, GLFW_KEY_RIGHT))
		{
			une::TransformSystem::Translate(e2, 2, 0, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_LEFT))
		{
			une::TransformSystem::Translate(e2, -2, 0, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_UP))
		{
			une::TransformSystem::Translate(e2, 0, 2, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_DOWN))
		{
			une::TransformSystem::Translate(e2, 0, -2, 0);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_PAGE_UP))
		{
			une::TransformSystem::Translate(e2, 0, 0, 1);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_PAGE_DOWN))
		{
			une::TransformSystem::Translate(e2, 0, 0, -1);
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_KP_ADD))
		{
			cam.Translate({0, 0, -10});
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_KP_SUBTRACT))
		{
			window->SetSize({800, 600});
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_W))
		{
			cam.Translate({0, 10, 0});
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_A))
		{
			cam.Translate({-10, 0, 0});
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_S))
		{
			cam.Translate({0, -10, 0});
		}
		if (glfwGetKey(window->glWindow, GLFW_KEY_D))
		{
			cam.Translate({10, 0, 0});
		}

		une::Update(&cam);
	}

	return 0;
}
