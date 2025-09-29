#pragma once

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Model.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"
#include "renderer/gl/Utils.h"

namespace une
{
	//3D ModelRenderer component
	ECS_REGISTER_COMPONENT(ModelRenderer)
	struct ModelRenderer
	{
		Model* model = nullptr;
		Shader* shader = nullptr;

		//Alternate textures, will override default ones from model
		std::vector<Texture*> textures;

		//Should this sprite be treated as a UI element, see doc/UserInterface.md
		bool uiElement = false;
		bool enabled = true;
	};

	namespace renderer
	{
		//3D ModelRenderSystem, requires Transform and ModelRenderer
		ECS_REGISTER_SYSTEM(ModelRenderSystem, Transform, ModelRenderer)
		class ModelRenderSystem : public ecs::System
		{
		public:
			//Initialize the shaders
			void Init();
			//Sorts the models into their draw layers
			void Prepass();
			//Draws all entities in the opaqueWorldEntities list
			void DrawOpaqueWorldEntities(Camera* cam);
			//Draws all entities in the opaqueUIEntities list, expects depth buffer to be reset
			void DrawOpaqueUIEntities(Camera* cam);
			//Draw an entity to the screen
			static void DrawEntity(ecs::Entity entity, Camera* cam);

			const std::vector<Renderable>& GetTransparentWorldEntities();
			const std::vector<Renderable>& GetTransparentUIEntities();

		private:
			static Shader* defaultShader;

			std::vector<ecs::Entity> opaqueWorldEntities;
			std::vector<ecs::Entity> opaqueUIEntities;
			std::vector<Renderable> transparentWorldEntities;
			std::vector<Renderable> transparentUIEntities;
		};
	}
}
