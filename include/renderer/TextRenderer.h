#pragma once

#include "renderer/gl/Camera.h"
#include "renderer/gl/Font.h"
#include "renderer/gl/Utils.h"
#include "Color.h"
#include "Transform.h"

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
		int size = 24;
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
			~TextRenderSystem();

			//Initialize the shaders
			void Init();

			//Sorts the text into their draw layers
			void Prepass();
			//Draw a sprite to the screen, expects bound VAO
			static void DrawEntity(ecs::Entity entity, Camera* cam);

			const std::vector<Renderable>& GetTransparentWorldEntities();
			const std::vector<Renderable>& GetTransparentUIEntities();

			static FT_Library ftLib;
		private:
			static Shader* shader;

			std::vector<Renderable> transparentWorldEntities;
			std::vector<Renderable> transparentUIEntities;
		};
	}
}
