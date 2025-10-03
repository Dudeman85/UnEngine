#pragma once

#include <vector>

#include <tmxlite/Map.hpp>
#include <glm/glm.hpp>

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
			unsigned int columns;
			unsigned int rows;
			Texture* texture = nullptr;
			Texture* lookup = nullptr;
		};

		//Create a subset for each tilseset this layer uses
		void CreateSubsets(const tmx::Map& map);

		std::vector<Texture*> tilesetTextures;
		std::vector<Subset> subsets;
	};
}
