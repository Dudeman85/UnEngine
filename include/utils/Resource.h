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
		//Get the asset path
		virtual std::string Path() const {return path;};
		//Does this texture contain editable data
		virtual bool Editable() const {return editable;};

	protected:
		std::string path;
		bool editable = false;
	};
}
