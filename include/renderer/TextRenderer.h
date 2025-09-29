#pragma once

#include "renderer/gl/Camera.h"
#include "Color.h"
#include "Font.h"
#include "Transform.h"
#include "gl/Utils.h"

namespace une
{
	//TextRenderer component
	ECS_REGISTER_COMPONENT(TextRenderer)
	struct TextRenderer
	{
		//The font of the text
		Font* font;
		//The text that is printed
		std::string text;
		//Font size
		int size = 12;
		//Color of the text
		Color color = Color(0, 0, 0, 255);
		//Should this text be treated as a UI element, see doc/UserInterface.md
		bool uiElement = false;
		bool enabled = true;
	};

	namespace renderer
	{
		//TextRenderSystem requires components TextRenderer and Transform
		ECS_REGISTER_SYSTEM(TextRenderSystem, Transform, TextRenderer)
		class TextRenderSystem : public ecs::System
		{
		public:
			//Initialize the shaders
			void Init();

			//Sorts the text into their draw layers
			void Prepass();
			//Draws all entities in the opaqueWorldEntities list
			void DrawOpaqueWorldEntities(Camera* cam);
			//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
			void DrawOpaqueUIEntities(Camera* cam);
			//Draw a sprite to the screen, expects bound VAO
			static void DrawEntity(ecs::Entity entity, Camera* cam);

			const std::vector<Renderable>& GetTransparentWorldEntities();
			const std::vector<Renderable>& GetTransparentUIEntities();

		private:
			static Shader* shader;

			std::vector<ecs::Entity> opaqueWorldEntities;
			std::vector<ecs::Entity> opaqueUIEntities;
			std::vector<Renderable> transparentWorldEntities;
			std::vector<Renderable> transparentUIEntities;
		};
	}
}
