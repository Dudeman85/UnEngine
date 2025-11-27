#include "renderer/UnifiedRenderer.h"

#include <algorithm>

#include "UnEngine.h"
#include "debug/Logging.h"

namespace une::renderer
{
	void Init()
	{
		//Set the screen clear color to black
		glClearColor(0, 0, 0, 1.0f);

		//Enable transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		//Enable Depth buffering
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		//MSAA
		glEnable(GL_MULTISAMPLE);

		primitiveRenderSystem->Init();
		spriteRenderSystem->Init();
		modelRenderSystem->Init();
		textRenderSystem->Init();
		tilemapRenderSystem->Init();
	}

	void UnifiedRenderPrepass()
	{
		primitiveRenderSystem->Prepass();
		spriteRenderSystem->Prepass();
		modelRenderSystem->Prepass();
		textRenderSystem->Prepass();
		tilemapRenderSystem->Prepass();

		debug::LogGLError();
	}

	void UnifiedRenderPass(ecs::Entity cameraEntity)
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		//First render all opaque world entities (non semi-transparent & non UI)
		primitiveRenderSystem->DrawOpaqueWorldEntities(cameraEntity);
		spriteRenderSystem->DrawOpaqueWorldEntities(cameraEntity);
		modelRenderSystem->DrawOpaqueWorldEntities(cameraEntity);

		glEnable(GL_BLEND);
		//Then sort all semi-transparent world entities and render them
		std::vector<std::vector<Renderable>> transparents = {
			primitiveRenderSystem->GetTransparentWorldEntities(),
			spriteRenderSystem->GetTransparentWorldEntities(),
			modelRenderSystem->GetTransparentWorldEntities(),
			textRenderSystem->GetTransparentWorldEntities(),
			tilemapRenderSystem->GetTransparentWorldLayers(),
		};
		//Reserve a vector for combined list of entities
		size_t size = 0;
		for (const std::vector<Renderable>& v : transparents)
			size += v.size();
		std::vector<Renderable> transparentEntites;
		transparentEntites.reserve(size);
		//Merge the vectors
		for (const std::vector<Renderable>& v : transparents)
			transparentEntites.insert(transparentEntites.end(), v.begin(), v.end());
		DrawOrderedEntities(transparentEntites, cameraEntity);

		//Clear depth buffer to always render UI above world
		glClear(GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		//Then render all opaque UI entities
		primitiveRenderSystem->DrawOpaqueUIEntities(cameraEntity);
		spriteRenderSystem->DrawOpaqueUIEntities(cameraEntity);
		modelRenderSystem->DrawOpaqueUIEntities(cameraEntity);

		glEnable(GL_BLEND);
		//Then sort all semi-transparent UI entities and render them
		transparents = {
			primitiveRenderSystem->GetTransparentUIEntities(),
			spriteRenderSystem->GetTransparentUIEntities(),
			modelRenderSystem->GetTransparentUIEntities(),
			textRenderSystem->GetTransparentUIEntities(),
		};
		//Reserve a vector for combined list of entities
		size = 0;
		for (const std::vector<Renderable>& v : transparents)
			size += v.size();
		std::vector<Renderable> transparentUIEntites;
		transparentUIEntites.reserve(size);
		//Merge the vectors
		for (const std::vector<Renderable>& v : transparents)
			transparentUIEntites.insert(transparentUIEntites.end(), v.begin(), v.end());
		DrawOrderedEntities(transparentUIEntites, cameraEntity);

		debug::LogGLError();
	}

	//Draws a list of renderable entities that need to be sorted based on distance
	void DrawOrderedEntities(std::vector<Renderable> entities, ecs::Entity cameraEntity)
	{
		Transform& t = ecs::GetComponent<Transform>(cameraEntity);

		//Calculate squared distances to camera
		for (Renderable& r: entities)
		{
			//TODO: This needs to take into account camera rotation
			r.distToCamera = t.position.z - r.position.z;
		}

		//Sort farthest to nearest based on relative distance to camera
		std::sort(entities.begin(), entities.end(),
			[](const Renderable& left, const Renderable& right)
		    {
				return left.distToCamera > right.distToCamera;
		    });

		//Draw them with the proper draw function
		for (const Renderable& r: entities)
		{
			r.render(r, cameraEntity);
		}
	}

	//Set the window clear color
	void SetBackgroundColor(const Color& c)
	{
		const Color srgb = c.AsSRGB();
		glClearColor(srgb.r, srgb.g, srgb.b, srgb.a);
	}
}
