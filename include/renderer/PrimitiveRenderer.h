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
		Primitive(const std::vector<double>& vertices, const std::vector<unsigned int>& indices, bool wf);

	public:
		Primitive() = delete;
		Primitive(const Primitive&) = delete;
		Primitive(Primitive&&);
		~Primitive();

		//Create a line starting at p1 and ending at p2
		static Primitive Line(Vector3 p1 = {-1, 0, 0}, Vector3 p2 = {1, 0, 0});
		//Create a triangle from three vertices, Defaults to equilateral triangle
		static Primitive Triangle(bool wireframe = false, Vector3 v1 = {-1, -1, 0}, Vector3 v2 = {1, -1, 0}, Vector3 v3 = {0, 1, 0});
		//Create a rectangle from four vertices going clockwise, Defaults to square
		static Primitive Rectangle(bool wireframe = false, Vector3 v1 = {-1, -1, 0}, Vector3 v2 = {-1, 1, 0}, Vector3 v3 = {1, 1, 0}, Vector3 v4 = {1, -1, 0});
		//Create a polygon from provided 3D vertices, going clockwise
		static Primitive Polygon(bool wireframe, const std::vector<Vector3>& verts);
		//Create a polygon from provided 2D vertices, going clockwise
		static Primitive Polygon(bool wireframe, const std::vector<Vector2>& verts, double z = 0);

		void Draw(const Color& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;

		int numVertices = 0;
		int numIndices = 0;
		bool wireframe = false;
		unsigned int VAO, VBO, EBO;
	};

	//Primitive Renderer Component, primitives consist of only a primitive shape and a color, no texture
	ECS_REGISTER_COMPONENT(PrimitiveRenderer)
	struct PrimitiveRenderer
	{
		Primitive* primitive = nullptr;
		Color color = {0};
		bool enabled = true;
	};

	namespace renderer
	{
		//Primitive Render system, Requires PrimitiveRenderer and Transform
		ECS_REGISTER_SYSTEM(PrimitiveRenderSystem, Transform, PrimitiveRenderer)
		class PrimitiveRenderSystem : public ecs::System
		{
		public:
			//Initialize the default shader
			void Init();

			//Sorts the primitives into their draw layers
			void Prepass();

			//Draws all entities in the opaqueWorldEntities list
			void DrawOpaqueWorldEntities(ecs::Entity cameraEntity);
			//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
			void DrawOpaqueUIEntities(ecs::Entity cameraEntity);
			//Draw a primitive to the screen
			void DrawEntity(ecs::Entity entity, ecs::Entity cameraEntity);
			//Static version of DrawEntity for renderable
			static void DrawRenderable(const Renderable& r, ecs::Entity cameraEntity);

			const std::vector<Renderable>& GetTransparentWorldEntities();
			const std::vector<Renderable>& GetTransparentUIEntities();

			static Shader* shader;
		private:

			std::vector<ecs::Entity> opaqueWorldEntities;
			std::vector<ecs::Entity> opaqueUIEntities;
			std::vector<Renderable> transparentWorldEntities;
			std::vector<Renderable> transparentUIEntities;
		};
	}
}