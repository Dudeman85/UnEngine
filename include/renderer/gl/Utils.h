#pragma once

#include <functional>

#include <glad/gl.h>

#include "ECS.h"
#include "renderer/gl/Camera.h"

namespace une::renderer
{
	//An entity with attached function on how to render it
	struct Renderable
	{
		ecs::Entity entity;
		std::function<void(ecs::Entity, Camera*)> render;
		float distToCamera = INFINITY;
	};

	//Better check gl error function
	GLenum CheckGLError();
}