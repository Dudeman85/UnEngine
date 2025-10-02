#pragma once

#include <vector>
#include <unordered_map>
#include <set>
#include <string>

#include "ECS.h"
#include "renderer/gl/MapLayer.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Camera.h"
#include "renderer/gl/Utils.h"
#include "Transform.h"
#include "Vector.h"

namespace une
{
	//A class to load a tiled tilemap using the tmxlite library
	class Tilemap
	{
	public:
		Tilemap(const std::string& path, unsigned int filteringType = GL_NEAREST);
		~Tilemap();

		void SetLayerVisibility(unsigned int id, bool visible);

		///Get the position of a tile in world coordinates
		Vector2 GetTilePosition(unsigned int x, unsigned int y);
		///Check larger area collisionbox
		std::vector<Vector2> CheckCollisionBox(Vector2 topLeft, Vector2 bottomRight);
		//Returns the id of every tile with a collider at tilemap coords x and y
		std::vector<unsigned int> GetCollisionTileAtLocation(unsigned int x, unsigned int y);
		///Check the smaller Collisions that are turned on upon the larger collision box colliding
		unsigned int checkCollision(float x, float y);
		//Returns the vertices making up this tile's collider
		std::vector<Vector2> GetTileCollider(unsigned int id);

		Vector2 tileSize;
		tmx::FloatRect bounds;

		std::unordered_map<unsigned int, std::vector<Vector2>> tileColliders;
		std::vector<MapLayer*> mapLayers;

	private:
		unsigned int VAO, VBO;
		std::vector<Texture*> tilesetTextures;
	};

	//Tilemap renderer component
	ECS_REGISTER_COMPONENT(TilemapRenderer)
	struct TilemapRenderer
	{
		Tilemap* tilemap;

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
			void DrawLayer(ecs::Entity entity, Camera* cam, unsigned int id);
			//Static version of DrawLayer for renderable
			static void DrawRenderable(const Renderable& r, Camera* cam);

			const std::vector<Renderable>& GetTransparentWorldLayers();

		private:
			Shader* shader = nullptr;
			std::vector<Renderable> transparentWorldLayers;
		};
	}
}