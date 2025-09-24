#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/gl.h>

namespace engine
{
	///Abstraction class for OpenGL shaders
	class Shader
	{
	public:
		///Give the vertex and fragment shader sources directly or if fromFile = true load them from given directories. 
		Shader(std::string vertexShaderPath, std::string fragmentShaderPath, bool fromFile = true);

		///Use this shader program
		void use();

		///the id num of the shaders
		unsigned int ID;
	};
}