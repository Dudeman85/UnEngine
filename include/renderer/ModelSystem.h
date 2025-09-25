#pragma once

#include <map>
#include <set>

#include "ECS.h"
#include "Transform.h"
#include "renderer/gl/Model.h"
#include "renderer/gl/Shader.h"
#include "renderer/gl/Texture.h"
#include "renderer/gl/Camera.h"

namespace une
{
	//3D Model Renderer component
	ECS_REGISTER_COMPONENT(ModelRenderer)
	struct ModelRenderer
	{
		//Stores vertex data
		Model* model;
		//Stores shader data
		Shader* shader;

		bool uiElement = false;

		bool enabled = true;

		//Alternate textures, will override default ones from model
		std::vector<Texture*> textures;
	};

	//3D Model Render System, requires Transform and ModelRenderer
	ECS_REGISTER_SYSTEM(ModelRenderSystem, Transform, ModelRenderer)
	class ModelRenderSystem : public ecs::System
	{
	public:
		///Initialize the Model Render System
		void Init();

		///Call this every frame
		void Update(Camera* cam);

		//This whole UI thing is becomming a disaster
		void DrawUIElements(Camera* cam) ;

		///Draw an entity to the screen
		void DrawEntity(ecs::Entity entity, Camera* cam);

		///Set light position and color
		void SetLight(Vector3 _lightPos, Vector3 _lightColor);

	private:
		//This is a goddamn mess
		std::set<float> uiLayersToDraw;
		std::map<float, std::vector<ecs::Entity>> sortedUIElements;

		Vector3 lightPos;
		Vector3 lightColor = Vector3(255);

		Shader* defaultShader = nullptr;
	};
}
