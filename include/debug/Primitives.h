#pragma once
#include "Color.h"
#include "ECS.h"
#include "Vector.h"

namespace debug
{
	//Should debug primitives be drawn over all world entities regardless of occlusion
	inline bool drawPrimitivesAboveAll = true;

	//Draw a Line
	void DrawLine(une::Vector3 p1, une::Vector3 p2, une::Color color);
	//Draw a Triangle
	void DrawTriangle(une::Vector3 p1, une::Vector3 p2, une::Vector3 p3, une::Color color, bool wireframe = false);
	//Draw a Rectangle
	void DrawRectangle(une::Vector3 p1, une::Vector3 p2, une::Vector3 p3, une::Vector3 p4, une::Color color, bool wireframe = false);
	//Draw a polygon
	void DrawPolygon(std::vector<une::Vector3> vertices, une::Color color, bool wireframe = false);

	void RenderAllDebugPrimitives(ecs::Entity cameraEntity);
}
