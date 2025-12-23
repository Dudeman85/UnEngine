#pragma once

#include "ECS.h"
#include "tmxlite/Map.hpp"

#ifndef __gl_h_
#include "glad/gl.h"
#endif

#include "Vector.h"
#include "renderer/gl/Texture.h"

namespace une
{
	//A single layer in the tilemap
	class MapLayer
	{
	public:
		MapLayer(const tmx::Map& map, uint32_t i, const std::vector<Texture*>& textures, std::unordered_map<std::string, tmx::Property> layerProperties);
		~MapLayer();
		MapLayer(const MapLayer&) = delete;
		MapLayer& operator = (const MapLayer&) = delete;

		//Draw a quad for each subset in this layer
		void DrawSubsets(int tilesetSizeLoc);

		uint32_t index;
		bool enabled = true;
		bool hasCollision = false;
		//User specified z offset
		float zOffset = 0;
		//Tile based collision map
		std::vector<std::vector<uint32_t>> collider;
		std::unordered_map<std::string, tmx::Property> properties;

	private:
		struct Subset
		{
			unsigned int columns = 0;
			unsigned int rows = 0;
			Texture* texture = nullptr;
			Texture* lookup = nullptr;
		};

		//Create a subset for each tilseset this layer uses
		void CreateSubsets(const tmx::Map& map);

		std::vector<Texture*> tilesetTextures;
		std::vector<Subset> subsets;
	};

	//A class to load a tiled tilemap using the tmxlite library
	class Tilemap
	{
	public:
		struct TileInfo
		{
			//The unique gid of this tile
			uint32_t gid = 0;
			//The tileset ID of this tile
			uint32_t id = 0;
			//The name of the parent tileset
			std::string tilesetName;
			//The collider vertices of this tile
			std::vector<Vector2> collider;
		};

		Tilemap(const std::string& path, unsigned int filteringType = GL_NEAREST);
		~Tilemap();

		void SetLayerVisibility(uint32_t layer, bool visible);
		//Get the position of a tile in world coordinates when attached to an entity
		Vector3 GetTilePosition(ecs::Entity entity, Vector2Int pos) const;
		//Returns the TileInfo of every tile with a collider at tilemap coords
		std::vector<TileInfo> GetCollisionTilesAtLocation(Vector2Int pos) const;
		//Returns the vertices making up this tile's collider
		std::vector<Vector2> GetTileCollider(uint32_t gid) const;

		bool Valid();

		Vector2Int tileSize;
		std::vector<MapLayer*> mapLayers;

		unsigned int VAO, VBO;
	private:
		std::vector<tmx::Tileset> tilesets;
		std::unordered_map<uint32_t, std::vector<Vector2>> tileColliders;
		std::vector<Texture*> tilesetTextures;
	};
}