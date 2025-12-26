#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "renderer/gl/Texture.h"
#include "renderer/gl/Mesh.h"

namespace une
{
	///A class to store 3D model vertex and texture data, as well as handle model loading
	class Model : public resources::Resource
	{
	public:
		Model() = default;
		~Model() override;

		bool Load(const std::string& path);
		bool SetupGLResources() override;

		bool Valid() const override {return valid;};

		//Model data
		std::vector<Mesh> meshes;

	private:
		void ProcessNode(aiNode* node);
		Mesh ProcessMesh(aiMesh* mesh);
		std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

		//Store all the already loaded textures for efficiency
		std::vector<Texture*> textures_loaded;

		bool valid = false;
		std::string directory;

		Assimp::Importer* importer = nullptr;
		const aiScene* scene = nullptr;
	};
}