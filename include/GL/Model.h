#pragma once
//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//STL 
#include <string>
#include <vector>

//Engine
#include <Constants.h>
#include <GL/Shader.h>
#include <GL/Texture.h>
#include <GL/Camera.h>
#include <GL/Mesh.h>

namespace engine
{
	///A class to store 3D model vertex and texture data, as well as handle model loading
	class Model
	{
	public:
		///Load the models from the asset path
		Model(const std::string& path)
		{
			LoadModel(path);
		}
		~Model()
		{
			for (Texture* tex : textures_loaded)
			{
				delete tex;
			}
		}

		///Model data
		std::vector<Mesh> meshes;

	private:
		void LoadModel(std::string path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

		//Model file path
		std::string directory;
		//Store all the already loaded textures for efficiency
		std::vector<Texture*> textures_loaded;
	};
}