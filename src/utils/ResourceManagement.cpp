#include "utils/ResourceManagement.h"

#include <filesystem>
#include <unordered_map>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"

namespace une::resources
{
    std::string rootPath = "";
    std::unordered_map<std::string, Resource*> resources;
    std::unordered_map<std::string, LoadingStatus> loadingResources;
    std::mutex loadingResourcesMutex;
    std::mutex resourcesMutex;

    //Thread safe functions
    void AppendLoadingResources(const std::string& path, LoadingStatus loadingStatus)
    {
        loadingResourcesMutex.lock();
        loadingResources[path] = loadingStatus;
        loadingResourcesMutex.unlock();
    }
    void AppendResources(const std::string& path, Resource* resource)
    {
        resourcesMutex.lock();
        resources[path] = resource;
        resourcesMutex.unlock();
    }
    void EraseLoadingResources(const std::string& path)
    {
        loadingResourcesMutex.lock();
        loadingResources.erase(path);
        loadingResourcesMutex.unlock();
    }
    void EraseResources(const std::string& path)
    {
        resourcesMutex.lock();
        resources.erase(path);
        resourcesMutex.unlock();
    }

    //Deal with asynchronously loaded resources
    void Update()
    {
        for (auto it = loadingResources.cbegin(); it != loadingResources.cend();)
        {
            //If resource is done loading
            if (resources.contains(it->first))
            {
                resources[it->first]->SetupGLResources();
                if (!resources[it->first]->Valid())
                {
                    EraseResources(it->first);
                    debug::LogError("Failed to setup OpenGL resources for " + it->first);
                }
                loadingResourcesMutex.lock();
                it = loadingResources.erase(it);
                loadingResourcesMutex.unlock();
            }
            else
            {
                ++it;
            }
        }
    }

    //Unloads a texture if it is loaded
    void Unload(std::string path)
    {
        path = rootPath + path;
        if (loadingResources.contains(path))
        {
            EraseLoadingResources(path);
        }
        if (resources.contains(path))
        {
            delete resources[path];
            EraseResources(path);
        }
    }

    //Preload a list of resources relative to rootPath, these can then be fetched with the appropriate Load function
    //Returns true if all resources were successfully loaded
    bool PreloadResources(const std::vector<std::string>& paths)
    {
        for (const std::string& path : paths)
        {
            AppendLoadingResources(rootPath + path, LoadingStatus::Queued);
        }

        bool success = true;
        for (const std::string& path : paths)
        {
            //Get the file extention
            std::string extension = "";
            const auto position = path.find_last_of('.');
            if (position != std::string::npos)
            {
                extension = path.substr(position + 1);
            }
            else
            {
                debug::LogWarning(path + " does not have a valid extension");
                success = false;
                continue;
            }

            //Load the appropriate type of model
            if (resourceLoadFuncs.contains(extension))
            {
                success &= resourceLoadFuncs.at(extension)(path);
            }
            else
            {
                debug::LogWarning(path + " is not a supported resource type");
                success = false;
            }
        }
        return success;
    }

    //Preload a list of resources relative to rootPath asynchronously, these can then be fetched with the appropriate Load function
    //Returns true if and when all resources were successfully loaded
    std::future<bool> PreloadResourcesAsync(const std::vector<std::string>& paths)
    {
        for (const std::string& path : paths)
        {
            AppendLoadingResources(rootPath + path, LoadingStatus::Queued);
        }

        return std::async(std::launch::async, [paths]()
        {
            bool success = true;
            std::vector<std::future<bool>> threads;
            for (const std::string& path : paths)
            {
                //Get the file extention
                std::string extension = "";
                const auto position = path.find_last_of('.');
                if (position != std::string::npos)
                {
                    extension = path.substr(position + 1);
                }
                else
                {
                    debug::LogWarning(path + " does not have a valid extension");
                    success = false;
                    continue;
                }

                //Load the appropriate type of model
                if (resourceLoadFuncs.contains(extension))
                {
                     success &= resourceLoadFuncs.at(extension)(path);
                }
                else
                {
                    debug::LogWarning(path + " is not a supported resource type");
                    success = false;
                }
            }

            return success;
        });
    }
}