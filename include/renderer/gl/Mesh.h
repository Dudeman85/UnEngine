#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "renderer/gl/Texture.h"

namespace une
{
	class Mesh
	{
	public:
		struct Vertex
		{
			glm::vec3 Position;
			glm::vec3 Normal;
			glm::vec2 TexCoords;
		};

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
		~Mesh();
		//Generate OpenGL buffers, needs to be called from main thread
		void SetupGLResources();

		//Mesh data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture*> textures;

		//Buffer Data
		unsigned int VAO, VBO, EBO;
	};
}