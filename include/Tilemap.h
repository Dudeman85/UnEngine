#pragma once

#include "glad/gl.h"
#include "tmxlite/Map.hpp"

#include "Vector.h"
#include "renderer/gl/Texture.h"

namespace une
{
	//A single layer in the tilemap
	class MapLayer
	{
	public:
		MapLayer(const tmx::Map& map, unsigned int i, const std::vector<Texture*>& textures, std::unordered_map<std::string, tmx::Property> layerProperties);
		~MapLayer();
		MapLayer(const MapLayer&) = delete;
		MapLayer& operator = (const MapLayer&) = delete;

		//Draw a quad for each subset in this layer
		void DrawSubsets(int tilesetSizeLoc);

		unsigned int index;
		bool enabled = true;
		//User specified z offset
		float zOffset = 0;
		//Tile based collision map
		std::vector<std::vector<unsigned int>> collider;
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
		Tilemap(const std::string& path, unsigned int filteringType = GL_NEAREST);
		~Tilemap();

		void SetLayerVisibility(unsigned int id, bool visible);

		//Get the position of a tile in world coordinates
		Vector3 GetTilePosition(unsigned int x, unsigned int y);
		//Check larger area collisionbox
		std::vector<Vector2> CheckCollisionBox(Vector2 topLeft, Vector2 bottomRight);
		//Returns the id of every tile with a collider at tilemap coords x and y
		std::vector<unsigned int> GetCollisionTileAtLocation(unsigned int x, unsigned int y);
		//Check the smaller Collisions that are turned on upon the larger collision box colliding
		unsigned int checkCollision(float x, float y);
		//Returns the vertices making up this tile's collider
		std::vector<Vector2> GetTileCollider(unsigned int id);

		Vector2Int tileSize;
		tmx::FloatRect bounds;

		std::unordered_map<unsigned int, std::vector<Vector2>> tileColliders;
		std::vector<MapLayer*> mapLayers;

		unsigned int VAO, VBO;
	private:
		std::vector<Texture*> tilesetTextures;
	};
}