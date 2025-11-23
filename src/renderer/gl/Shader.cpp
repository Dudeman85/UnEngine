#include "renderer/gl/Shader.h"

#include "debug/Logging.h"
#include "debug/DebugTiming.h"


namespace une
{
	//Give the vertex and fragment shader sources directly or if fromFile = true load them from given directories.
	Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool fromFile)
	{
		debug::StartTimer("LoadShader");

		//Load Vertex shader
		std::string vertexShaderString;
		const char* vertexShaderSource;

		if (fromFile)
		{
			//Load the Vertex shader from file if given
			std::ifstream file(vertexShaderPath);
			if (!file)
			{
				debug::LogError("Failed to load vertex shader from " + vertexShaderPath);
				return;
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			vertexShaderString = buffer.str();
			vertexShaderSource = vertexShaderString.c_str();
		}
		else
		{
			//Load the Vertex shader from string
			vertexShaderSource = vertexShaderPath.c_str();
		}

		//Create and compile the vertex shader
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		//Check for error in compiling the vertex shader
		int  success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			debug::LogError("Failed to compile vertex shader:\n" + std::string(infoLog));
			return;
		}

		//Load Fragment Shader
		std::string fragmentShaderString;
		const char* fragmentShaderSource;

		if (fromFile)
		{
			//Load the Fragment shader from file if given
			std::ifstream file(fragmentShaderPath);
			if (!file)
			{
				debug::LogError("Failed to load fragment shader from " + fragmentShaderPath);
				return;
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			fragmentShaderString = buffer.str();
			fragmentShaderSource = fragmentShaderString.c_str();
		}
		else
		{
			//Load the Fragment shader from string
			fragmentShaderSource = fragmentShaderPath.c_str();
		}

		//Create and compile the fragment shader
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		//Check for error in compiling the fragment shader
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			debug::LogError("Failed to compile fragment shader:\n" + std::string(infoLog));
			return;
		}

		//Create the shader program
		ID = glCreateProgram();
		//Attach the fragment and vertex shaders to the program
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		//Check for errors linking the shaders
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			debug::LogError("Failed to link shaders:\n" + std::string(infoLog));
			return;
		}

		//Delete the shader programs after they are no longer needed
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		debug::LogSpam("Successfully compiled shader from " + (fromFile ? fragmentShaderPath : "code") + " in "
			+ std::to_string(debug::EndTimer("LoadShader")) + "ms");
	}

	//Use this shader program in the bound texture slot
	void Shader::Use() const
	{
		glUseProgram(ID);
	}
}
