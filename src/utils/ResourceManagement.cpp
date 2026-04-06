#include "utils/ResourceManagement.h"

#include <filesystem>
#include <unordered_map>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Model.h"

namespace une::resources
{
    std::mutex resourcesMutex;

    //Thread safe helper functions
    void AppendResources(const std::string& path, std::shared_ptr<Resource> resource)
    {
        std::lock_guard<std::mutex> lock(resourcesMutex);
        resources[path] = resource;
    }
    void EraseResources(const std::string& path)
    {
        std::lock_guard<std::mutex> lock(resourcesMutex);
        resources.erase(path);
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
    //Unsafe disables resource mutex and should be set if unloading nested resources 
    void Unload(std::string path, bool unsafe)
    {
        path = rootPath + path;

        if (resources.contains(path))
        {
            auto test = resources[path].use_count();
            if (resources[path].use_count() <= 1)
            {
                if (unsafe)
                {
                    resources.erase(path);
                }
                else
                {
                    EraseResources(path);
                }
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
                success &= (bool)resourceLoadFuncs.at(extension)(path);
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
                     success &= (bool)resourceLoadFuncs.at(extension)(path);
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