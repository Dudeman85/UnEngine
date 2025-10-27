#include "renderer/UnifiedRenderer.h"

#include <algorithm>

#include "UnEngine.h"
#include "Debug.h"

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
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		primitiveRenderSystem->Prepass();
		spriteRenderSystem->Prepass();
		modelRenderSystem->Prepass();
		textRenderSystem->Prepass();
		tilemapRenderSystem->Prepass();

		debug::CheckGLError();
	}

	void UnifiedRenderPass(Camera* cam)
	{
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_BLEND);
		//First render all opaque world entities (non semi-transparent & non UI)
		primitiveRenderSystem->DrawOpaqueWorldEntities(cam);
		spriteRenderSystem->DrawOpaqueWorldEntities(cam);
		modelRenderSystem->DrawOpaqueWorldEntities(cam);

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
		for (std::vector<Renderable> v : transparents)
			size += v.size();
		std::vector<Renderable> transparentEntites;
		transparentEntites.reserve(size);
		//Merge the vectors
		for (std::vector<Renderable> v : transparents)
			transparentEntites.insert(transparentEntites.end(), v.begin(), v.end());
		DrawOrderedEntities(transparentEntites, cam);

		//Clear depth buffer to always render UI above world
		glClear(GL_DEPTH_BUFFER_BIT);

		glDisable(GL_BLEND);
		//Then render all opaque UI entities
		primitiveRenderSystem->DrawOpaqueUIEntities(cam);
		spriteRenderSystem->DrawOpaqueUIEntities(cam);
		modelRenderSystem->DrawOpaqueUIEntities(cam);

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
		for (std::vector<Renderable> v : transparents)
			size += v.size();
		std::vector<Renderable> transparentUIEntites;
		transparentUIEntites.reserve(size);
		//Merge the vectors
		for (std::vector<Renderable> v : transparents)
			transparentUIEntites.insert(transparentUIEntites.end(), v.begin(), v.end());
		DrawOrderedEntities(transparentUIEntites, cam);

		debug::CheckGLError();
	}

	//Draws a list of renderable entities that need to be sorted based on distance
	void DrawOrderedEntities(std::vector<Renderable> entities, Camera* cam)
	{
		//Calculate squared distances to camera
		for (Renderable& r: entities)
		{
			//TODO: This needs to take into account camera rotation
			r.distToCamera = cam->position.z - r.position.z;
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
			r.render(r, cam);
		}
	}

	//Set the window clear color
	void SetBackgroundColor(Color c)
	{
		Color srgb = c.AsSRGB();
		glClearColor(srgb.r, srgb.g, srgb.b, srgb.a);
	}
}
