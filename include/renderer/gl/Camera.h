#pragma once

#include <glm/glm.hpp>

#include "ECS.h"
#include "Transform.h"
#include "utils/Types.h"
#include "Vector.h"

namespace une
{

	ECS_REGISTER_COMPONENT(Camera)
	struct Camera
	{
		bool perspective = false;
		//Width and height are used with an orthographic camera
		float width, height;
		//Field of view is used with a perspective camera
		float fov = 75;
		float nearPlane = 0.1;
		float farPlane = 10000;
		//Cameras with a lower number draw to the canvas first
		int drawOrder = 0;
		//The area of the screen to draw on. From bottom-left(0, 0) to top-right(1, 1) in ndc.
		Rect viewport = {0, 0, 1, 1};

		bool enabled = true;

		glm::mat4 view;
		glm::mat4 projection;
	};

	ECS_REGISTER_SYSTEM(CameraSystem, Camera, Transform)
	class CameraSystem : public ecs::System
	{
	public:
		//Renders all camera views to the canvas
		void Update();

		static void MakeOrtho(ecs::Entity e, float width, float height, float nearPlane = 0.1f, float farPlane = 10000);
		static void MakePerspective(ecs::Entity e, float fov, float nearPlane = 0.1f, float farPlane = 10000);
		//Recalculates the view matrix, called automatically when transform changes
		static void RecalculateView(ecs::Entity e);
		//Recalculates the projection matrix, call this if you manually change fov, perspectve, nearPlane, or farPlane
		static void RecalculateProjection(ecs::Entity e);
	};
}