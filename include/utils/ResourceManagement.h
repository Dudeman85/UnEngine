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
    //These should be treated as read only
    extern std::unordered_map<std::string, Resource*> resources;
    enum class LoadingStatus {Queued, Loading, Ready};
    extern std::unordered_map<std::string, LoadingStatus> loadingResources;

    //Thread safe functions
    void AppendLoadingResources(const std::string& path, LoadingStatus loadingStatus);
    void AppendResources(const std::string& path, Resource* resource);
    void EraseLoadingResources(const std::string& path);
    void EraseResources(const std::string& path);

    //Load or fetch a resource, also sets up its OpenGl resources
    template<typename T, typename... Types>
    T* Load(std::string path, Types... args)
    {
        path = rootPath + path;
        //Load from storage if not already loaded
        if (!resources.contains(path))
        {
            if (loadingResources.contains(path))
            {
                debug::LogWarning(path + " is already being loaded asynchronously!");
                return nullptr;
            }

            AppendLoadingResources(path, LoadingStatus::Loading);

            //Load from disk
            T* resource = new T();
            if (!resource->Load(path, args...))
            {
                delete resource;
                return nullptr;
            }
            //Make the OpenGL resources
            resource->SetupGLResources();
            if (!resource->Valid())
            {
                delete resource;
                return nullptr;
            }

            EraseLoadingResources(path);
            AppendResources(path, resource);
            return resource;
        }

        //If right type, cast and return
        T* ret = dynamic_cast<T*>(resources[path]);
        if (!ret)
        {
            debug::LogError(path + " is not of type " + std::string(typeid(T).name()));
        }
        return ret;
    }

    //Load or fetch a resource, does not setup OpenGL resources
    template<typename T, typename... Types>
    T* BasicLoad(std::string path, Types... args)
    {
        path = rootPath + path;
        //Load from storage if not already loaded
        if (!resources.contains(path))
        {
            if (loadingResources.contains(path))
            {
                if (loadingResources[path] != LoadingStatus::Queued)
                {
                    debug::LogWarning(path + " is already being loaded asynchronously!");
                    return nullptr;
                }
            }

            AppendLoadingResources(path, LoadingStatus::Loading);

            //Load from disk
            T* resource = new T();
            if (!resource->Load(path, args...))
            {
                delete resource;
                EraseLoadingResources(path);
                return nullptr;
            }

            AppendLoadingResources(path, LoadingStatus::Ready);
            AppendResources(path, resource);
            return resource;
        }

        //If right type, cast and return
        T* ret = dynamic_cast<T*>(resources[path]);
        if (!ret)
        {
            debug::LogError(path + " is not of type " + std::string(typeid(T).name()));
        }
        return ret;
    }

    //Asynchronously Load or fetch a resource, OpenGl resources will only be setup the frame after the resource has loaded
    template<typename T, typename... Types>
    [[nodiscard]] std::future<T*> LoadAsync(const std::string& path, Types... args)
    {
        std::string fullPath = rootPath + path;
        //Immediately return if already loaded
        if (resources.contains(fullPath))
        {
            return std::async(std::launch::async, [path, args...](){return BasicLoad<T>(path, args...);});
        }

        //Currently being loaded, return pointer when ready
        if (loadingResources.contains(fullPath))
        {
            return std::async(std::launch::async, [fullPath, path, args...]()
            {
                while (loadingResources.contains(fullPath))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
                return BasicLoad<T>(path, args...);
            });
        }

        //Start a thread to load this
        AppendLoadingResources(fullPath, LoadingStatus::Queued);
        return std::async(std::launch::async, [fullPath, args...]() -> T*
        {
            AppendLoadingResources(fullPath, LoadingStatus::Loading);
            T* resource = new T();
            resource->Load(fullPath, args...);
            //If this has been unloaded during loading, delete it
            if (!loadingResources.contains(fullPath))
            {
                delete resource;
                return nullptr;
            }
            AppendLoadingResources(fullPath, LoadingStatus::Ready);
            AppendResources(fullPath, resource);
            return resource;
        });
    }

    inline const std::unordered_map<std::string, std::function<bool(std::string)>> resourceLoadFuncs{
            {"png", BasicLoad<Texture>},
            {"obj", BasicLoad<Model>},
            {"ttf", BasicLoad<Font>}, {"otf", BasicLoad<Font>},
            {"tmx", BasicLoad<Tilemap>},
        };

    //Deal with asynchronously loaded resources
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