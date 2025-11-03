#pragma once

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"
#include "renderer/gl/Utils.h"

namespace une
{
	//2D SpriteRenderer component
	ECS_REGISTER_COMPONENT(SpriteRenderer)
	struct SpriteRenderer
	{
		Texture* texture = nullptr;
		Shader* shader = nullptr;

		bool enabled = true;
	};

	namespace renderer
	{
		//2D SpriteRenderSystem, Requires SpriteRenderer and Transform
		ECS_REGISTER_SYSTEM(SpriteRenderSystem, SpriteRenderer, Transform)
		class SpriteRenderSystem : public ecs::System
		{
		public:
			~SpriteRenderSystem();
			//Initialize the shaders and shared buffers
			void Init();
			//Sorts the sprites into their draw layers
			void Prepass();
			//Draws all entities in the opaqueWorldEntities list
			void DrawOpaqueWorldEntities(ecs::Entity cameraEntity);
			//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
			void DrawOpaqueUIEntities(ecs::Entity cameraEntity);
			//Draw a sprite to the screen, expects bound VAO
			void DrawEntity(ecs::Entity entity, ecs::Entity cameraEntity);
			//Static version of DrawEntity for renderable
			static void DrawRenderable(const Renderable& r, ecs::Entity cameraEntity);

			const std::vector<Renderable>& GetTransparentWorldEntities();
			const std::vector<Renderable>& GetTransparentUIEntities();

		private:
			Shader* defaultShader = nullptr;
			unsigned int VAO, VBO;

			std::vector<ecs::Entity> opaqueWorldEntities;
			std::vector<ecs::Entity> opaqueUIEntities;
			std::vector<Renderable> transparentWorldEntities;
			std::vector<Renderable> transparentUIEntities;
		};
	}
}