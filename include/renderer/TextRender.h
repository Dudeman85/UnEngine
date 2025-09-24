#pragma once

#include "renderer/gl/Camera.h"
#include "Font.h"
#include "Transform.h"

namespace engine
{
	/// TextRenderer component
	ECS_REGISTER_COMPONENT(TextRenderer)
	struct TextRenderer
	{
		///The font of the text
		Font* font;
		///The text that is printed
		std::string text = "";
		///Location of the text on the screen
		Vector3 offset;
		///Rotation of the text
		Vector3 rotation;
		///Size of the text
		//Vector2 charRes;
		// Old
		Vector3 scale = Vector3(1);
		///Color of the text
		Vector3 color = Vector3(0);
		///Bool to turn on the ui elements
		bool uiElement = false;
	};

	/// TextRenderSystem requires components TextRenderer component and transform component
	ECS_REGISTER_SYSTEM(TextRenderSystem, Transform, TextRenderer)
	class TextRenderSystem : public ecs::System
	{
	public:
		///Initialize the shaders
		void Init();

		///Call this every frame
		void Update(Camera* cam);

	private:
		Shader* m_shader = nullptr;
	};
}