#include <iostream>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "renderer/gl/TilemapLayer.h"

#include "Debug.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Utils.h"

namespace une
{
	MapLayer::MapLayer(const tmx::Map& map, unsigned int i, const std::vector<Texture*>& textures, std::unordered_map<std::string, tmx::Property> layerProperties)
	{
		tilesetTextures = textures;
		index = i;
		properties = layerProperties;

		//Set some properties
		enabled = map.getLayers()[i]->getVisible();
		//If layer has a specified z offset set it here
		if (properties.contains("zoffset"))
			zOffset = properties["zoffset"].getFloatValue();

		//If the layer has collision enabled give it a collider
		if (properties.contains("zoffset"))
		{
			if (properties["collision"].getBoolValue())
			{
				//Get the tile IDs
				auto& tiles = map.getLayers()[i]->getLayerAs<tmx::TileLayer>().getTiles();

				//Transfer the tile IDs to the 2D collision vector
				collider.resize(map.getTileCount().x);
				for (int x = 0; x < map.getTileCount().x; x++)
				{
					collider[x].resize(map.getTileCount().y);
					for (int y = 0; y < map.getTileCount().y; y++)
					{
						collider[x][y] = tiles[y * collider.size() + x].ID;
					}
				}
			}
		}

		CreateSubsets(map);
	}

	MapLayer::~MapLayer()
	{
		for (auto& ss: subsets)
		{
			delete ss.lookup;
		}
	}

	//Draw a quad for each subset in this layer
	void MapLayer::DrawSubsets(int tilesetSizeLoc)
	{
		for (const auto& ss: subsets)
		{
			glUniform2ui(tilesetSizeLoc, ss.columns, ss.rows);

			glActiveTexture(GL_TEXTURE0);
			ss.texture->Use();
			glActiveTexture(GL_TEXTURE1);
			ss.lookup->Use();

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	//Create a subset for each tilseset this layer uses
	void MapLayer::CreateSubsets(const tmx::Map& map)
	{
		const auto& layers = map.getLayers();
		if (index >= layers.size() || layers[index]->getType() != tmx::Layer::Type::Tile)
		{
			debug::LogWarning("Invalid tilemap layer index or layer type, layer will be empty");
			return;
		}

		const tmx::Vector2u& mapSize = map.getTileCount();
		const auto& tiles = layers[index]->getLayerAs<tmx::TileLayer>().getTiles();
		//Go through all tilesets in the map
		for (int i = 0; i < map.getTilesets().size(); i++)
		{
			const tmx::Tileset& ts = map.getTilesets()[i];
			std::vector<uint16_t> lookupData;
			bool tsUsed = false;

			//Check each tile ID to see if it falls in the current tile set
			for (const tmx::TileLayer::Tile& tile : tiles)
			{
				if (tile.ID >= ts.getFirstGID() && tile.ID <= ts.getLastGID())
				{
					//Make sure to index relative to the tileset
					uint16_t id = tile.ID - ts.getFirstGID();
					//Red channel is used for tile ids
					lookupData.push_back(id);
					//Green channel is used for flip flags
					lookupData.push_back(tile.flipFlags);
					tsUsed = true;
				}
				else
				{
					//UINT16_MAX aka 65535 is no tile
					lookupData.push_back(UINT16_MAX);
					lookupData.push_back(UINT16_MAX);
				}
			}

			//If we have some data for this tile set, create the resources
			if (tsUsed)
			{
				//Make the lookup texture
				unsigned int tex;
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				//Only nearest works
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				//Give the lookup table data
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16UI, mapSize.x, mapSize.y, 0, GL_RG_INTEGER, GL_UNSIGNED_SHORT, lookupData.data());
				glBindTexture(GL_TEXTURE_2D, 0);

				//Add the subset
				Subset subset{
					ts.getColumnCount(),
					ts.getTileCount() / ts.getColumnCount(),
					tilesetTextures[i],
					new Texture(tex)
				};
				subsets.push_back(subset);
			}
		}
	}
}
