#pragma once

#include <fstream>
#include <string>

namespace une
{
	//Abstraction class for OpenGL shaders
	class Shader
	{
	public:
		//Give the vertex and fragment shader sources directly or if fromFile = true load them from given directories.
		Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool fromFile = true);

		//Use this shader program in the bound texture slot
		void Use() const;

		unsigned int ID;
	};
}