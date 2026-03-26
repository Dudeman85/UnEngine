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
    extern std::string rootPath;
    //Map of resource paths to resources. This should be treated as read only
    extern std::unordered_map<std::string, Resource*> resources;

    //Thread safe helper functions
    void AppendResources(const std::string& path, Resource* resource);
    void EraseResources(const std::string& path);

    //Load or fetch a resource, does not setup OpenGL resources.
    //addUser can be set to prevent premature unloading in the case of shared resources
    template<typename T, typename... Types>
    T* BasicLoad(std::string path, bool addUser = false, Types... args)
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
            T* ret = dynamic_cast<T*>(resources[fullPath]);
            if (!ret)
            {
                debug::LogError(fullPath + " is not of type " + std::string(typeid(T).name()));
                return nullptr;
            }
            if (addUser)
                resources[fullPath]->users++;
            return ret;
        }

        T* resource = new T();
        resource->status = Resource::Status::Loading;
        if (addUser)
            resource->users++;
        AppendResources(fullPath, resource);

        //Load from disk
        if (!resource->Load(fullPath, args...))
        {
            delete resource;
            return nullptr;
        }

        resource->status = Resource::Status::Loaded;
        return resource;
    }

    //Load or fetch a resource, also sets up its OpenGl resources, therefore only call this from the main thread
    //addUser can be set to prevent premature unloading in the case of shared resources
    template<typename T, typename... Types>
    T* Load(std::string path, bool addUser = false, Types... args)
    {
        std::string fullPath = rootPath + path;

        //Immediately return if already loaded
        if (resources.contains(fullPath))
        {
            if (resources[fullPath]->status == Resource::Status::Ready)
            {
                //If right type, cast and return
                T* ret = dynamic_cast<T*>(resources[fullPath]);
                if (!ret)
                {
                    debug::LogError(fullPath + " is not of type " + std::string(typeid(T).name()));
                    return nullptr;
                }
                if (addUser)
                    resources[fullPath]->users++;
                return ret;
            }
        }

        T* resource = BasicLoad<T>(path, addUser, args...);

        //Make the OpenGL resources
        resource->SetupGLResources();
        if (!resource->Valid())
        {
            delete resource;
            return nullptr;
        }

        resource->status = Resource::Status::Ready;
        return resource;
    }

    //Asynchronously Load or fetch a resource, OpenGl resources will only be setup the frame after the resource has loaded
    //addUser can be set to prevent premature unloading in the case of shared resources
    template<typename T, typename... Types>
    [[nodiscard]] std::future<T*> LoadAsync(const std::string& path, bool addUser = false, Types... args)
    {
        std::string fullPath = rootPath + path;
        if (resources.contains(fullPath))
        {
            //Immediately return if already loaded
            if (resources[fullPath]->status == Resource::Status::Ready)
            {
                return std::async(std::launch::deferred, [path, addUser, args...]()
                {
                    return BasicLoad<T>(path, addUser, args...);
                });
            }

            //Currently being loaded, return pointer when ready
            if (resources[fullPath]->status == Resource::Status::Loading)
            {
                return std::async(std::launch::async, [fullPath, path, addUser, args...]()
                {
                    while (resources[fullPath]->status == Resource::Status::Loading)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(16));
                    }
                    return BasicLoad<T>(path, addUser, args...);
                });
            }
        }

        //Start a thread to load this
        return std::async(std::launch::async, [fullPath, addUser, args...]() -> T*
        {
            T* resource = new T();
            resource->status = Resource::Status::Loading;
            if (addUser)
                resource->users++;
            AppendResources(fullPath, resource);

            resource->Load(fullPath, args...);

            //If this has been unloaded during loading, delete it
            if (!resources.contains(fullPath))
            {
                delete resource;
                return nullptr;
            }
            return resource;
        });
    }

    inline const std::unordered_map<std::string, std::function<bool(std::string, bool)>> resourceLoadFuncs{
            {"png", BasicLoad<Texture>},
            {"obj", BasicLoad<Model>},
            {"ttf", BasicLoad<Font>}, {"otf", BasicLoad<Font>},
            {"tmx", BasicLoad<Tilemap>},
        };

    //Setup opengl stuff of asunchronously loaded resources
    void Update();
    //Unloads a texture if it is loaded
    void Unload(std::string path);
    //Preload a list of resources relative to rootPath, these can then be fetched with the appropriate Load function
    //Returns true if all resources were successfully loadedon
    bool PreloadResources(const std::vector<std::string>& paths);
    //Preload a list of resources relative to rootPath asynchronously, these can then be fetched with the appropriate Load function
    //Returns true if and when all resources were successfully loaded
    [[nodiscard]] std::future<bool> PreloadResourcesAsync(const std::vector<std::string>& paths);
}