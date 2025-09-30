#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "ECS.h"
#include "renderer/gl/MapLayer.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Camera.h"
#include "Transform.h"
#include "Vector.h"

namespace une
{
	//A class to load a tiled tilemap using the tmxlite library
	class Tilemap
	{
	public:
		///Load tilemap
		void loadMap(const std::string& ownMap, unsigned int filteringType = GL_NEAREST);

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
		///The size of a tile
		tmx::Vector2u tileSize;
		///The position of a tile
		glm::vec3 position;
		///The bounds of the map
		tmx::FloatRect bounds;

		std::unordered_map<unsigned int, std::vector<Vector2>> tileColliders;

		//All of these should be modes to MapLayer
		std::unordered_map<unsigned int, bool> enabledLayers;
		std::unordered_map<unsigned int, float> layerZs;
		std::unordered_map<unsigned int, std::unordered_map<std::string, tmx::Property>> layerProperties;
		std::unordered_map<unsigned int, std::vector<std::vector<unsigned int>>> layerColliders;

	private:

		std::unordered_map<float, std::vector<std::shared_ptr<MapLayer>>> mapLayers;
		std::vector<Texture> tilesetTextures;
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

			static void DrawEntity(ecs::Entity entity, Camera* cam);

		private:
			static Shader* shader;
		};
	}
}