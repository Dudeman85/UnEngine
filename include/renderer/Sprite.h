#pragma once

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"
#include "renderer/Tilemap.h"

namespace engine
{
	///2D Sprite Renderer component
	ECS_REGISTER_COMPONENT(SpriteRenderer)
	struct SpriteRenderer
	{
		///Abstraction class for textures
		Texture* texture = nullptr;
		///Abstraction class for shaders
		Shader* shader = nullptr;
		///Bool to turn on the sprite renderer
		bool enabled = true;
		///Bool to turn on the ui elements
		bool uiElement = false;
	};

	///2D Sprite Render system, Requires SpriteRenderer and Transform
	ECS_REGISTER_SYSTEM(SpriteRenderSystem, SpriteRenderer, Transform)
	class SpriteRenderSystem : public ecs::System
	{
	public:
		~SpriteRenderSystem();
		///Initialize the shaders and clear the screen
		void Init();

		///Renders everything. Call this every frame
		void Update(Camera* cam);

		///Draw an entity to the screen
		void DrawEntity(ecs::Entity entity, Camera* cam);

		///Set the screens clear color to given rgb
		static void SetBackgroundColor(float r, float g, float b);

		//Set a tilemap to render
		void SetTilemap(Tilemap* map);
		//Remove a tilemap from rendering
		void RemoveTilemap();

	private:
		unsigned int VAO, VBO, EBO;
		Shader* defaultShader = nullptr;
		Tilemap* tilemap = nullptr;
	};
}