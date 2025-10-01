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

		///Draw the map with the given data
		void draw(glm::mat4 model, int modelLoc, int, int);

		unsigned int index;
		bool enabled = true;
		//User specified z offset
		float zOffset = 0;
		Vector2 tileSize;
		//Tile based collision map
		std::vector<std::vector<unsigned int>> collider;
		std::unordered_map<std::string, tmx::Property> properties;

	private:
		struct Subset
		{
			float sx;
			float sy;
			unsigned int vbo = 0;
			Texture* texture = 0;
			Texture* lookup = 0;
		};

		void CreateSubsets(const tmx::Map&, std::size_t);

		std::vector<Texture*> tilesetTextures;
		std::vector<Subset> subsets;
		unsigned int VAO;
	};
}
