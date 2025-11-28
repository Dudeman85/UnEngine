#pragma once

#include <vector>
#include <unordered_map>
#include <set>
#include <string>

#include "ECS.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Utils.h"
#include "Tilemap.h"
#include "Transform.h"

namespace une
{
	//Tilemap renderer component
	ECS_REGISTER_COMPONENT(TilemapRenderer)
	struct TilemapRenderer
	{
		Tilemap* tilemap = nullptr;

		bool enabled = true;
	};

	namespace renderer
	{
		ECS_REGISTER_SYSTEM(TilemapRenderSystem, Transform, TilemapRenderer)
		class TilemapRenderSystem : public ecs::System
		{
		public:
			void Init();

			//Sorts the tilemap layers into their draw layers (currently only transparent world)
			void Prepass();

			//Draws one layer of an entity's tilemap
			void DrawLayer(ecs::Entity entity, ecs::Entity cameraEntity, unsigned int id);
			//Static version of DrawLayer for renderable
			static void DrawRenderable(const Renderable& r, ecs::Entity cameraEntity);

			const std::vector<Renderable>& GetTransparentWorldLayers();

		private:
			Shader* shader = nullptr;
			std::vector<Renderable> transparentWorldLayers;
		};
	}
}