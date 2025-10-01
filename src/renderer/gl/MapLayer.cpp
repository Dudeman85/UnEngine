#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "renderer/gl/MapLayer.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Utils.h"

namespace une
{
    MapLayer::MapLayer(const tmx::Map& map, unsigned int i, const std::vector<Texture*>& textures, std::unordered_map<std::string, tmx::Property> layerProperties)
    {
        tilesetTextures = textures;
        index = i;
        properties = layerProperties;
        tileSize = Vector2(map.getTileSize().x, map.getTileSize().y);

        //Set some properties
        enabled = map.getLayers()[i]->getVisible();
        //If layer has a specified z offset set it here
        if (layerProperties.contains("zoffset"))
            zOffset = layerProperties["zoffset"].getFloatValue();

        //If the layer has collision enabled give it a collider
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

        CreateSubsets(map, i);
    }

    MapLayer::~MapLayer()
    {
        for(auto& ss : subsets)
        {
            glDeleteBuffers(1, &ss.vbo);
            delete ss.lookup;
        }
    }

    void MapLayer::draw(glm::mat4 model, int modelLoc, int tilesetCountLoc, int tileSizeLoc)
    {
        if (subsets.empty())
            return;

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, subsets[0].vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zOffset));
        model = glm::rotate(model, (float)M_PI, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glUniform2f(tileSizeLoc, tileSize.x, tileSize.y);

        glBindVertexArray(VAO);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        for(const auto& ss : subsets)
        {
            glBindBuffer(GL_ARRAY_BUFFER, ss.vbo);

            glUniform2f(tilesetCountLoc, ss.sx, ss.sy);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ss.texture->ID());

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ss.lookup->ID());

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void MapLayer::CreateSubsets(const tmx::Map& map, std::size_t layerIdx)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        const auto& layers = map.getLayers();
        if(layerIdx >= layers.size() || (layers[layerIdx]->getType() != tmx::Layer::Type::Tile))
        {
            std::cout << "Invalid layer index or layer type, layer will be empty" << std::endl;
            return;
        }
        const auto layer = dynamic_cast<const tmx::TileLayer*>(layers[layerIdx].get());

        auto bounds = map.getBounds();
        float verts[] =
        {
            bounds.left, bounds.top, 0.f, 0.f, 0.f,
            bounds.left + bounds.width, bounds.top, 0.f, 1.f, 0.f,
            bounds.left, bounds.top + bounds.height, 0.f, 0.f, 1.f,
            bounds.left + bounds.width, bounds.top + bounds.height, 0.f, 1.f, 1.f
        };

        const auto& mapSize = map.getTileCount();
        const auto& tilesets = map.getTilesets();
        for(auto i = 0u; i < tilesets.size(); ++i)
        {
            //check each tile ID to see if it falls in the current tile set
            const auto& ts = tilesets[i];
            int numRows = ts.getTileCount() / ts.getColumnCount();
            int numCols = ts.getColumnCount();
            const auto& tileIDs = layer->getTiles();
            std::vector<std::uint16_t> pixelData;
            bool tsUsed = false;

            for(auto y = 0u; y < mapSize.y; ++y)
            {
                for(auto x = 0u; x < mapSize.x; ++x)
                {
                    auto idx = y * mapSize.x + x;
                    if (idx < tileIDs.size() && tileIDs[idx].ID >= ts.getFirstGID()
                         && tileIDs[idx].ID < (ts.getFirstGID() + ts.getTileCount()))
                    {
                        auto id = tileIDs[idx].ID - ts.getFirstGID() + 1;
                        pixelData.push_back(static_cast<std::uint16_t>(id)); //red channel - making sure to index relative to the tileset
                        pixelData.push_back(static_cast<std::uint16_t>(tileIDs[idx].flipFlags)); //green channel - tile flips are performed on the shader
                        tsUsed = true;
                    }
                    else
                    {
                        //pad with empty space
                        pixelData.push_back(0);
                        pixelData.push_back(0);
                    }
                }
            }

            //if we have some data for this tile set, create the resources
            if(tsUsed)
            {
                subsets.emplace_back();
                subsets.back().sx = numCols;
                subsets.back().sy = numRows;
                subsets.back().texture = tilesetTextures[i];
                subsets.back().lookup = new Texture(mapSize.x, mapSize.y, pixelData);

                glGenBuffers(1, &subsets.back().vbo);
                glBindBuffer(GL_ARRAY_BUFFER, subsets.back().vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
            }
        }
    }
}