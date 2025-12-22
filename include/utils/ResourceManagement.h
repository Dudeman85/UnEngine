#pragma once

#include <unordered_map>
#include <future>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"
#include "renderer/gl/Font.h"
#include "Tilemap.h"

namespace une::resources
{
    static std::string resourcePath = "";

    //Loads a texture or fetches it if already loaded
    static Texture* LoadTexture(std::string path);
    //Loads a texture asynchronously or fetches it if already loaded, texture is only valid on success
    //Returns -1 on failure, 0 on incomplete and 1 on success
    static std::future<Texture*> LoadTextureAsync(std::string path);
    
    //Loads a model or fetches it if already loaded
    static Model* LoadModel (std::string path);
    //Loads a model asynchronously or fetches it if already loaded, model is only valid on success
    //Returns -1 on failure, 0 on incomplete and 1 on success
    static std::future<Model*> LoadModelAsync(std::string path);

    //Loads a font or fetches it if already loaded
    static Font* LoadFont(std::string path, int resolution);
    //Loads a font asynchronously or fetches it if already loaded, font is only valid on success
    //Returns -1 on failure, 0 on incomplete and 1 on success
    static std::future<Font*> LoadFontAsync(std::string path, int resolution);

    //Loads a tilemap or fetches it if already loaded
    static Tilemap* LoadTilemap(std::string path);
    //Loads a tilemap asynchronously or fetches it if already loaded, tilemap is only valid on success
    //Returns -1 on failure, 0 on incomplete and 1 on success
    static std::future<Tilemap*> LoadTilemapAsync(std::string path);


    //Recursively loads all textures 
    std::unordered_map<std::string, Texture*> ProcessDirectoryTextures(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR);

    //Recursively loads all models
    std::unordered_map<std::string, Model*> ProcessDirectoryModels(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR);

    //Load every texture from path, if includeSubdirectories is specified also recursively loads subdirectories
    //Returns a map with the name of the texture's path relative to root directory, ex. "level1/Player.png"
    std::unordered_map<std::string, Texture*> PreloadTextures(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR);

    //Load every model from path, if includeSubdirectories is specified also recursively loads subdirectories
    //Returns a map with the name of the models's path relative to root directory, ex. "level1/Player.obj"
    std::unordered_map<std::string, Model*> PreloadModels(const std::string& path, bool includeSubdirectories = true, unsigned int filteringType = GL_LINEAR);
}