#include "debug/Primitives.h"

#include "gl.h"

#include "renderer/PrimitiveRenderer.h"

namespace debug
{
	std::vector<std::pair<une::Primitive, une::Color>> debugPrimitives;

	//Draw a Line
	void DrawLine(une::Vector3 p1, une::Vector3 p2, une::Color color)
	{
		une::Primitive primitive = une::Primitive::Line(p1, p2);
		debugPrimitives.push_back({ std::move(primitive), color });
	}

	//Draw a Triangle
	void DrawTriangle(une::Vector3 p1, une::Vector3 p2, une::Vector3 p3, une::Color color, bool wireframe)
	{
		une::Primitive primitive = une::Primitive::Triangle(wireframe, p1, p2, p3);
		debugPrimitives.push_back({ std::move(primitive), color });
	}

	//Draw a Rectangle
	void DrawRectangle(une::Vector3 p1, une::Vector3 p2, une::Vector3 p3, une::Vector3 p4, une::Color color, bool wireframe)
	{
		une::Primitive primitive = une::Primitive::Rectangle(wireframe, p1, p2, p3, p4);
		debugPrimitives.push_back({ std::move(primitive), color });
	}

	//Draw a polygon
	void DrawPolygon(const std::vector<une::Vector3>& vertices, une::Color color, bool wireframe)
	{
		une::Primitive primitive = une::Primitive::Polygon(wireframe, vertices);
		debugPrimitives.push_back({ std::move(primitive), color });
	}
	void DrawPolygon(const std::vector<une::Vector2>& vertices, une::Color color, bool wireframe, double z)
	{
		une::Primitive primitive = une::Primitive::Polygon(wireframe, vertices, z);
		debugPrimitives.push_back({ std::move(primitive), color });
	}

	void RenderAllDebugPrimitives(ecs::Entity cameraEntity)
	{
		une::Camera& cam = ecs::GetComponent<une::Camera>(cameraEntity);

		if (drawPrimitivesAboveAll)
			glDisable(GL_DEPTH_TEST);

		for (const auto& primitive : debugPrimitives)
		{
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 view = cam.view;
			glm::mat4 projection = cam.projection;

			primitive.first.Draw(primitive.second, model, view, projection);
		}

		glEnable(GL_DEPTH_TEST);
		debugPrimitives.clear();
	}
}
