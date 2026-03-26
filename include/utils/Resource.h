#pragma once
#include <string>

namespace une::resources
{
	class Resource
	{
	public:
		enum class Status {Invalid, Loading, Loaded, Ready};

		virtual ~Resource() = default;
		virtual bool SetupGLResources() {return false;}
		//Is the OpenGl texture usable
		virtual bool Valid() const {return false;}
		//Get the asset path relative to rootPath
		virtual std::string Path() const {return path;}
		//Get the absolute asset path
		virtual std::string FullPath() const {return fullPath;}
		//Does this resource contain editable data
		virtual bool Editable() const {return editable;}

		//Get the Loading status
		Status status = Status::Invalid;
		//How many users this resource has
		int users = 0;
	protected:
		std::string path;
		std::string fullPath;
		bool editable = false;
	};
}
