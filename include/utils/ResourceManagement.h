#pragma once

#include <unordered_map>
#include <future>
#include <variant>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"
#include "renderer/gl/Font.h"
#include "Tilemap.h"

namespace une::resources
{
    struct Resource
    {
        void* ptr;
        std::string type;
    };

    //Path to load resources relative to
    static std::string resourcePath = "";

    //These should be treated as read only
    static std::unordered_map<std::string, Resource> resources;
    static std::unordered_map<std::string, bool> loadingResources;

    template<typename T, typename... Types>
    T* LoadResource(const std::string& path, Types... args)
    {
        //Load from storage if not already loaded
        if (!resources.contains(path))
        {
            if (loadingResources.contains(path))
            {
                debug::LogWarning(path + " is already being loaded asynchronously!");
                return nullptr;
            }

            T* resource = new T(path, args...);
            //Check successful load
            if (!resource->Valid())
            {
                delete resource;
                return nullptr;
            }
            resources[path] = Resource(resource, typeid(T).name());
        }

        //If right type, cast and return
        if (resources[path].type == typeid(T).name())
        {
            return (T*)resources[path].ptr;
        }
        else
        {
            debug::LogError(path + " is not of type " + std::string(typeid(T).name()));
            return nullptr;
        }
    }

    template<typename T, typename... Types>
    std::future<T*> LoadResourceAsync(const std::string& path, Types... args)
    {
        //Immediately return if already loaded
        if (resources.contains(path))
        {
            return std::async(std::launch::deferred, [path, args...](){return LoadResource<T>(path, args...);});
        }

        //Currently being loaded, return pointer when ready
        if (loadingResources.contains(path))
        {
            return std::async(std::launch::async, [path, args...]()
            {
                while (loadingResources.contains(path))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
                return LoadResource<T>(path, args...);
            });
        }

        //Start a thread to load this
        loadingResources[path] = true;
        return std::async(std::launch::async, [path, args...]() -> T*
        {
            T* resource = new T(path, args...);
            //Check successful load
            if (!resource->Valid())
            {
                delete resource;
                return nullptr;
            }
            resources[path] = Resource(resource, typeid(T).name());

            //If this has been unloaded during loading, delete it
            if (!loadingResources.contains(path))
            {
                delete resource;
                return nullptr;
            }

            resources[path] = Resource(resource, typeid(T).name());
            loadingResources.erase(path);
            return resource;
        });
    }

    //Unloads a texture if it is loaded
    template<typename T>
    void UnloadResource(const std::string& path)
    {
        if (loadingResources.contains(path))
        {
            loadingResources.erase(path);
        }
        else
        {
            if (resources.contains(path))
            {
                //If right type, cast and delete
                if (resources[path].type == typeid(T).name())
                {
                    delete (T*)resources[path].ptr;
                    resources.erase(path);
                }
                else
                {
                    debug::LogWarning("Failed to delete resource: " + path + " is not of type " + std::string(typeid(T).name()));
                }
            }
        }
    }

    Texture* LoadTexture(const std::string& path, unsigned int filteringType = GL_NEAREST, bool flip = true);
    std::future<Texture*> LoadTextureAsync(const std::string& path, unsigned int filteringType = GL_NEAREST, bool flip = true);

    Model* LoadModel(const std::string& path);
    std::future<Model*> LoadModelAsync(const std::string& path);

    Font* LoadFont(const std::string& path, unsigned short resolution);
    std::future<Font*> LoadFontAsync(const std::string& path, unsigned short resolution);

    Tilemap* LoadTilemap(const std::string& path, unsigned int filteringType = GL_NEAREST);
    std::future<Tilemap*> LoadTilemapAsync(const std::string& path, unsigned int filteringType = GL_NEAREST);
}