#pragma once

#include <vector>
#include <memory>

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

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::weak_ptr<Texture>>& textures);
		Mesh(const Mesh&) = delete; 
		Mesh(Mesh&&) noexcept = default;
		~Mesh();
		//Generate OpenGL buffers, needs to be called from main thread
		void SetupGLResources();

		//Mesh data
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::weak_ptr<Texture>> textures;

		//Buffer Data
		unsigned int VAO = 0, VBO = 0, EBO = 0;
	};
}