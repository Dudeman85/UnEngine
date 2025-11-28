#include "Tilemap.h"

#include <algorithm>

#include "tmxlite/Map.hpp"
#include "tmxlite/TileLayer.hpp"

#include "Vector.h"
#include "debug/Logging.h"
#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Window.h"

namespace une
{
	MapLayer::MapLayer(const tmx::Map& map, uint32_t i, const std::vector<Texture*>& textures, std::unordered_map<std::string, tmx::Property> layerProperties)
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
		if (properties.contains("collision"))
		{
			if (properties["collision"].getBoolValue())
			{
				//Get the tile GIDs
				auto& tiles = map.getLayers()[i]->getLayerAs<tmx::TileLayer>().getTiles();
				hasCollision = true;
				//Transfer the tile GIDs to the 2D collision vector
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

	Tilemap::Tilemap(const std::string& path, unsigned int filteringType)
	{
		tmx::Map map;
		map.load(path);

		tileSize = Vector2(map.getTileSize().x, map.getTileSize().y);
		tilesets = map.getTilesets();

		//Process all tiles in each tileset
		for (const tmx::Tileset& tileset : tilesets)
		{
			//Load the tileset's texture
			tilesetTextures.push_back(new Texture(tileset.getImagePath(), filteringType, false));

			//Get the collider shapes for each tile id
			for (const tmx::Tileset::Tile& tile: tileset.getTiles())
			{
				if (!tile.objectGroup.getObjects().empty())
				{
					//Only one collider per tile is currently supported
					//Get the first object in a tile to be used as a collider
					for (const tmx::Vector2f& point: tile.objectGroup.getObjects()[0].getPoints())
					{
						//Transform the Vertex to work with our coordinate system
						Vector2 vertex(point.x, point.y);
						vertex.y = -vertex.y;
						vertex += Vector2((double)tileSize.x / 2, (double)tileSize.y / 2);
						tileColliders[tile.ID].push_back(vertex);
					}
				}
			}
		}

		//Process all the layers
		const auto& layers = map.getLayers();
		for (auto i = 0u; i < layers.size(); ++i)
		{
			//Currently we only support tile layers
			if (layers[i]->getType() == tmx::Layer::Type::Tile)
			{
				//Process the layer properties
				std::unordered_map<std::string, tmx::Property> layerProperties;
				for (const tmx::Property& property: layers[i]->getProperties())
				{
					//Convert all properties to lower case
					std::string propertyName;
					std::transform(property.getName().begin(), property.getName().end(), propertyName.begin(), tolower);
					layerProperties[propertyName] = property;
				}

				mapLayers.push_back(new MapLayer(map, i, tilesetTextures, layerProperties));
			}
		}

		//Initialize the OpenGL resources
		const tmx::FloatRect bounds = map.getBounds();
		const float vertices[] = {
			bounds.left, bounds.top, 0.f, 0.f, 1.f,
			bounds.left + bounds.width, bounds.top, 0.f, 1.f, 1.f,
			bounds.left, bounds.top + bounds.height, 0.f, 0.f, 0.f,
			bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 0.f
		};

		//Make the single shared buffer
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		//Every layer will use the same vertex data
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		//Vertex positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		//Lookup texture coords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Tilemap::~Tilemap()
	{
		if (mainWindow)
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteVertexArrays(1, &VBO);
		}
		for (Texture* tex : tilesetTextures)
			delete tex;
		for (MapLayer* layer : mapLayers)
			delete layer;
	}

	void Tilemap::SetLayerVisibility(uint32_t layer, bool visible)
	{
		if (layer >= mapLayers.size())
		{
			debug::LogWarning("Invalid layer id " + std::to_string(layer));
			return;
		}

		mapLayers[layer]->enabled = visible;
	}

	//Get the position of a tile in world coordinates when attached to an entity
	Vector3 Tilemap::GetTilePosition(ecs::Entity entity, Vector2Int pos) const
	{
		if (!ecs::HasComponent<Transform>(entity))
		{
			debug::LogWarning("Entity " + std::to_string(entity) + " does not have a transform");
			return Vector3();
		}

		//Move from top left to center position
		glm::vec4 position(0);
		position.x = (float)(pos.x * tileSize.x) + (float)tileSize.x / 2.f;
		position.y = -(float)(pos.y * tileSize.y) - (float)tileSize.y / 2.f;
		position = TransformSystem::GetGlobalTransformMatrix(entity) * position;

		return Vector3(position.x, position.y, position.z);
	}

	//Returns the vertices making up this tile's collider
	std::vector<Vector2> Tilemap::GetTileCollider(uint32_t gid) const
	{
		if (tileColliders.contains(gid - 1))
		{
			return tileColliders.at(gid - 1);
		}
		else
		{
			//Default collider of a tile
			return std::vector<Vector2>{
				Vector2(-((float) tileSize.x / 2), (float) tileSize.y / 2), //Top-Left
				Vector2((float) tileSize.x / 2, (float) tileSize.y / 2), //Top-Right
				Vector2((float) tileSize.x / 2, -((float) tileSize.y / 2)), //Bottom-Right
				Vector2(-((float) tileSize.x / 2), -((float) tileSize.y / 2)) //Bottom-Left
			};
		}
	}

	//Returns the TileInfo of every tile with a collider at tilemap coords
	std::vector<Tilemap::TileInfo> Tilemap::GetCollisionTilesAtLocation(Vector2Int pos) const
	{
		std::vector<TileInfo> hits;
		for (const MapLayer* layer : mapLayers)
		{
			if (layer->hasCollision && layer->collider[pos.x][pos.y] != 0)
			{
				TileInfo info{layer->collider[pos.x][pos.y]};
				info.collider = tileColliders.at(info.gid);

				//Find the tileset this GID belongs to
				for (int i = 0; i < tilesets.size(); i++)
				{
					if (tilesets[i].getFirstGID() > info.gid)
					{
						i--;
						info.id = info.gid - tilesets[i].getFirstGID();
						info.tilesetName = tilesets[i].getName();
						hits.push_back(info);
						break;
					}
				}
			}
		}
		return hits;
	}
}
