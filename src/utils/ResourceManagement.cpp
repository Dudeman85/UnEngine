#include "utils/ResourceManagement.h"

#include <filesystem>
#include <unordered_map>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"

namespace une::resources
{
    std::string rootPath = "";
    std::unordered_map<std::string, Resource*> resources;
    std::mutex resourcesMutex;

    //Thread safe helper functions
    void AppendResources(const std::string& path, Resource* resource)
    {
        resourcesMutex.lock();
        resources[path] = resource;
        resourcesMutex.unlock();
    }
    void EraseResources(const std::string& path)
    {
        resourcesMutex.lock();
        resources.erase(path);
        resourcesMutex.unlock();
    }

    //Setup opengl stuff of asunchronously loaded resources
    void Update()
    {
        for (auto resource : resources)
        {
            //If resource is done loading setup gl resources in main thread
            if (resource.second->status == Resource::Status::Loaded)
            {
                resource.second->SetupGLResources(); 
                resource.second->status = Resource::Status::Ready;
                if (!resource.second->Valid())
                {
                    EraseResources(resource.first);
                    debug::LogError("Failed to setup OpenGL resources for " + resource.first);
                }
            }
        }
    }

    //Unloads a resource if it is loaded and has no other users
    void Unload(std::string path)
    {
        path = rootPath + path;

        if (resources.contains(path))
        {
            if (--resources[path]->users <= 0)
            {
                delete resources[path];
                EraseResources(path);
            }
        }
    }

    //Preload a list of resources relative to rootPath, these can then be fetched with the appropriate Load function
    //Returns true if all resources were successfully loaded
    bool PreloadResources(const std::vector<std::string>& paths)
    {
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
                success &= resourceLoadFuncs.at(extension)(path, false);
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
                     success &= resourceLoadFuncs.at(extension)(path, false);
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