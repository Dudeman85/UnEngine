#pragma once

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"
#include "renderer/Tilemap.h"

namespace une
{
	//2D Sprite Renderer component
	ECS_REGISTER_COMPONENT(SpriteRenderer)
	struct SpriteRenderer
	{
		Texture* texture = nullptr;
		Shader* shader = nullptr;
		//Should this sprite be treated as a UI element, see doc/UserInterface.md
		bool uiElement = false;
		bool enabled = true;
	};

	//2D Sprite Render system, Requires SpriteRenderer and Transform
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
		void DrawOpaqueWorldEntities(Camera* cam);
		//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
		void DrawOpaqueUIEntities(Camera* cam);
		//Draw a sprite to the screen, expects bound VAO
		void DrawEntity(ecs::Entity entity, Camera* cam);

		const std::vector<ecs::Entity>& GetTransparentWorldEntities();
		const std::vector<ecs::Entity>& GetTransparentUIEntities();

	private:
		unsigned int VAO, VBO, EBO;
		Shader* defaultShader = nullptr;

		std::vector<ecs::Entity> opaqueWorldEntities;
		std::vector<ecs::Entity> transparentWorldEntities;
		std::vector<ecs::Entity> opaqueUIEntities;
		std::vector<ecs::Entity> transparentUIEntities;
	};
}