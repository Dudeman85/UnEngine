#pragma once

#include <vector>

#include "ECS.h"
#include "Transform.h"
#include "Color.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Camera.h"
#include "renderer/gl/Utils.h"

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
		Color color;
		//Draw wireframe only
		bool wireframe = true;
		//Should this sprite be treated as a UI element, see doc/UserInterface.md
		bool uiElement = false;
		bool enabled = true;
	};

	namespace renderer
	{
		//Primitive Render system, Requires PrimitiveRenderer and Transform
		ECS_REGISTER_SYSTEM(PrimitiveRenderSystem, Transform, PrimitiveRenderer)
		class PrimitiveRenderSystem : public ecs::System
		{
		public:
			enum DrawPriority{ normal = 0, ui = 1, aboveAll = 2 };

			//Initialize the default shader
			void Init();

			//Sorts the primitives into their draw layers
			void Prepass();

			//Draws all entities in the opaqueWorldEntities list
			void DrawOpaqueWorldEntities(Camera* cam);
			//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
			void DrawOpaqueUIEntities(Camera* cam);
			//Draw a primitive to the screen
			static void DrawEntity(ecs::Entity entity, Camera* cam);
			//Draw a primitive to the screen, does not require an entity
			static void DrawPrimitive(const Primitive* primitive, Camera* cam, const Color& color, DrawPriority prio, Vector3 position = 0, Vector3 rotation = 0, Vector3 scale = 1);

			const std::vector<Renderable>& GetTransparentWorldEntities();
			const std::vector<Renderable>& GetTransparentUIEntities();

		private:
			static Shader* shader;

			std::vector<ecs::Entity> opaqueWorldEntities;
			std::vector<ecs::Entity> opaqueUIEntities;
			std::vector<Renderable> transparentWorldEntities;
			std::vector<Renderable> transparentUIEntities;
		};
	}
}