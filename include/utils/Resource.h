#pragma once
#include <string>

namespace une::resources
{
	class Resource
	{
	public:
		virtual ~Resource() = default;
		virtual bool SetupGLResources() {return false;};
		//Is the OpenGl texture usable
		virtual bool Valid() const {return false;};
		//Get the asset path relative to rootPath
		virtual std::string Path() const {return path;};
		//Get the absolute asset path
		virtual std::string FullPath() const {return fullPath;};
		//Does this resource contain editable data
		virtual bool Editable() const {return editable;};

	protected:
		std::string path;
		std::string fullPath;
		bool editable = false;
	};
}
