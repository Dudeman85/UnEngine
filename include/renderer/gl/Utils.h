#pragma once

#include <functional>

#include "ECS.h"
#include "renderer/gl/Camera.h"

namespace une::renderer
{
	//A renderable entity with data and a function on how to render it
	struct Renderable
	{
		ecs::Entity entity = 0;
		Vector3 position;
		std::function<void(Renderable, Camera*)> render;

		//Extra data
		unsigned int index = 0; //Tilemap

		//Renderer implementation internal
		float distToCamera = INFINITY;
	};
}