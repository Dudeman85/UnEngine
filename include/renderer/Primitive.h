#pragma once

#include <vector>

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Camera.h"

namespace une
{
	//A class to create and store a primitive shape
	class Primitive
	{
	private:
		Primitive(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

	public:
		Primitive() = delete;
		~Primitive();

		//A primitive can be drawn without being an entity
		void Draw(Camera* cam, Vector3 color = Vector3(100.0f), const Transform& transform = Transform()) const;

		//Create a line starting at p1 and ending at p2
		static Primitive Line(Vector3 p1 = Vector3(-1, 0, 0), Vector3 p2 = Vector3(1, 0, 0));
		//Create a triangle from three vertices, Defaults to equilateral triangle
		static Primitive Triangle(Vector3 v1 = Vector3(-1, -1, 0), Vector3 v2 = Vector3(1, -1, 0), Vector3 v3 = Vector3(0, 1, 0));
		//Create a rectangle from four vertices going clockwise, Defaults to square
		static Primitive Rectangle(Vector3 v1 = Vector3(-1, -1, 0), Vector3 v2 = Vector3(-1, 1, 0), Vector3 v3 = Vector3(1, 1, 0), Vector3 v4 = Vector3(1, -1, 0));
		//Create a polygon from provided 3D vertices, going clockwise
		static Primitive Polygon(const std::vector<Vector3>& verts);
		//Create a polygon from provided 2D vertices, going clockwise
		static Primitive Polygon(const std::vector<Vector2>& verts);

		int numVertices = 0;
		unsigned int VAO, VBO, EBO;
	};

	//Primitive Renderer Component, primitives consist of only a primitive shape and a color, no texture
	ECS_REGISTER_COMPONENT(PrimitiveRenderer)
	struct PrimitiveRenderer
	{
		Primitive* primitive = nullptr;
		Vector3 color;
		//Draw wireframe only
		bool wireframe = true;
		//Should this sprite be treated as a UI element, see doc/UserInterface.md
		bool uiElement = false;
		bool enabled = true;
	};

	//Primitive Render system, Requires PrimitiveRenderer and Transform
	ECS_REGISTER_SYSTEM(PrimitiveRenderSystem, Transform, PrimitiveRenderer)
	class PrimitiveRenderSystem : public ecs::System
	{
	public:
		//Initialize the default shader
		void Init();

		//Call this every frame
		void Update(Camera* cam);

		//Draw a primitive to the screen
		void DrawEntity(ecs::Entity entity, Camera* cam);
		//Draw a primitive to the screen, does not require an entity
		void DrawEntity(const Primitive& primitive, Camera* cam, Vector3 position, Vector3 rotation, Vector3 scale, Vector3 color);

		const std::vector<ecs::Entity>& GetTransparentWorldEntities();
		const std::vector<ecs::Entity>& GetTransparentUIEntities();

	private:
		Shader* defaultShader = nullptr;

		std::vector<ecs::Entity> opaqueWorldEntities;
		std::vector<ecs::Entity> transparentWorldEntities;
		std::vector<ecs::Entity> opaqueUIEntities;
		std::vector<ecs::Entity> transparentUIEntities;
	};
}