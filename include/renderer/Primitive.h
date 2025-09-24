#pragma once

#include <vector>

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Camera.h"

namespace engine
{
	//A class to create and store a primitive shape
	class Primitive
	{
	private:
		Primitive(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);

	public:
		~Primitive();

		//A primitive can be drawn without being an entity
		void Draw(Camera* cam, Vector3 color = Vector3(100.0f), const Transform& transform = Transform());

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

		unsigned int numVertices = 0;
		unsigned int VAO, VBO, EBO;
		static Shader* defaultShader;
	};

	ECS_REGISTER_COMPONENT(PrimitiveRenderer)
	//Primitive Renderer Component, They consist of only a primitive shape and a color, no texture
	struct PrimitiveRenderer
	{
		Primitive* primitive = nullptr;
		Vector3 color;
		//Draw wireframe only
		bool wireframe = true;

		bool lockXYRotation = false;
		bool enabled = true;
		bool uiElement = false;
	};

	ECS_REGISTER_SYSTEM(PrimitiveRenderSystem, Transform, PrimitiveRenderer)
	//Primitive Render system, Requires PrimitiveRenderer and Transform
	class PrimitiveRenderSystem : public ecs::System
	{
	public:
		//Initialize the shaders
		void Init();

		//Call this every frame
		void Update(Camera* cam);

		Shader* defaultShader = nullptr;
	};
}