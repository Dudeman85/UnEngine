#pragma once

#include <unordered_map>
#include <future>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"
#include "renderer/gl/Font.h"
#include "Tilemap.h"

namespace une::resources
{
    //Path to load resources relative to
    inline std::string rootPath = "";
    //Map of resource paths to resources. This should be treated as read only
    inline std::unordered_map<std::string, std::shared_ptr<Resource>> resources;

    //Thread safe helper functions
    void AppendResources(const std::string& path, std::shared_ptr<Resource> resource);
    void EraseResources(const std::string& path);

    //Load or fetch a resource, does not setup OpenGL resources.
    template<typename T, typename... Types>
    std::shared_ptr<T> BasicLoad(std::string path, Types... args)
    {
        std::string fullPath = rootPath + path;

        //Immediately return if already loaded
        if (resources.contains(fullPath))
        {
            if (resources[fullPath]->status == Resource::Status::Loading)
            {
                debug::LogWarning(fullPath + " is already being loaded asynchronously!");
                return nullptr;
            }

            //If right type, cast and return
            std::shared_ptr<T> ret = dynamic_pointer_cast<T>(resources[fullPath]);
            if (!ret)
            {
                debug::LogError(fullPath + " is not of type " + std::string(typeid(T).name()));
                return nullptr;
            }
            return ret;
        }

        std::shared_ptr<T> resource = std::make_shared<T>();
        resource->status = Resource::Status::Loading;
        AppendResources(fullPath, resource);

        //Load from disk
        if (!resource->Load(fullPath, args...))
        {
            EraseResources(fullPath);
            return nullptr;
        }

        resource->status = Resource::Status::Loaded;
        return resource;
    }

    //Load or fetch a resource, also sets up its OpenGl resources, therefore only call this from the main thread
    template<typename T, typename... Types>
    std::shared_ptr<T> Load(std::string path, Types... args)
    {
        std::string fullPath = rootPath + path;

        //Immediately return if already loaded
        if (resources.contains(fullPath))
        {
            if (resources[fullPath]->status == Resource::Status::Ready)
            {
                //If right type, cast and return
                std::shared_ptr<T> ret = dynamic_pointer_cast<T>(resources[fullPath]);
                if (!ret)
                {
                    debug::LogError(fullPath + " is not of type " + std::string(typeid(T).name()));
                    return nullptr;
                }
                return ret;
            }
        }

        std::shared_ptr<T> resource = BasicLoad<T>(path, args...);
        if (!resource)
        {
            return nullptr;
        }

        //Make the OpenGL resources
        resource->SetupGLResources();
        if (!resource->Valid())
        {
            EraseResources(fullPath);
            return nullptr;
        }

        resource->status = Resource::Status::Ready;
        return resource;
    }

    //Asynchronously Load or fetch a resource, OpenGl resources will only be setup the frame after the resource has loaded
    template<typename T, typename... Types>
    [[nodiscard]] std::future<std::shared_ptr<T>> LoadAsync(const std::string& path, Types... args)
    {
        std::string fullPath = rootPath + path;
        if (resources.contains(fullPath))
        {
            //Immediately return if already loaded
            if (resources[fullPath]->status == Resource::Status::Ready)
            {
                return std::async(std::launch::async, [path, args...]()
                {
                    return BasicLoad<T>(path, args...);
                });
            }

            //Currently being loaded, return pointer when ready
            if (resources[fullPath]->status == Resource::Status::Loading)
            {
                return std::async(std::launch::async, [fullPath, path, args...]()
                {
                    while (resources[fullPath]->status == Resource::Status::Loading)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(16));
                    }
                    return BasicLoad<T>(path, args...);
                });
            }
        }

        //Start a thread to load this
        return std::async(std::launch::async, [fullPath, args...]() -> std::shared_ptr<T>
        {
            std::shared_ptr<T> resource = std::make_shared<T>();
            resource->status = Resource::Status::Loading;
            AppendResources(fullPath, resource);

            resource->Load(fullPath, args...);

            //If this has been unloaded during loading, delete it
            if (!resources.contains(fullPath))
            {
                EraseResources(fullPath);
                return nullptr;
            }
            resource->status = Resource::Status::Loaded;
            return resource;
        });
    }

    inline const std::unordered_map<std::string, std::function<std::shared_ptr<Resource>(std::string)>> resourceLoadFuncs{
            {"png", BasicLoad<Texture>},
            {"obj", BasicLoad<Model>},
            {"ttf", BasicLoad<Font>}, {"otf", BasicLoad<Font>},
            {"tmx", BasicLoad<Tilemap>},
        };

    //Setup opengl stuff of asunchronously loaded resources
    void Update();
    //Unloads a texture if it is loaded
    void Unload(std::string path, bool unsafe = false);
    //Preload a list of resources relative to rootPath, these can then be fetched with the appropriate Load function
    //Returns true if all resources were successfully loadedon
    bool PreloadResources(const std::vector<std::string>& paths);
    //Preload a list of resources relative to rootPath asynchronously, these can then be fetched with the appropriate Load function
    //Returns true if and when all resources were successfully loaded
    [[nodiscard]] std::future<bool> PreloadResourcesAsync(const std::vector<std::string>& paths);
}