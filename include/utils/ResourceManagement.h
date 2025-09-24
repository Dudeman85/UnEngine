#pragma once

#include <unordered_map>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"

namespace engine
{
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