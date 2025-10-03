#include "renderer/UnifiedRenderer.h"

#include <algorithm>

#include "UnEngine.h"

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
        std::vector<Renderable> transparentPrimitives = primitiveRenderSystem->GetTransparentWorldEntities();
        std::vector<Renderable> transparentSprites = spriteRenderSystem->GetTransparentWorldEntities();
        std::vector<Renderable> transparentModels = modelRenderSystem->GetTransparentWorldEntities();
        std::vector<Renderable> transparentText = textRenderSystem->GetTransparentWorldEntities();
        std::vector<Renderable> transparentTilemapLayers = tilemapRenderSystem->GetTransparentWorldLayers();
        std::vector<Renderable> transparentEntites;
        transparentEntites.reserve(transparentPrimitives.size() + transparentSprites.size() + transparentModels.size() + transparentText.size() + transparentTilemapLayers.size());
        transparentEntites.insert(transparentEntites.end(), transparentPrimitives.begin(), transparentPrimitives.end());
        transparentEntites.insert(transparentEntites.end(), transparentSprites.begin(), transparentSprites.end());
        transparentEntites.insert(transparentEntites.end(), transparentModels.begin(), transparentModels.end());
        transparentEntites.insert(transparentEntites.end(), transparentText.begin(), transparentText.end());
        transparentEntites.insert(transparentEntites.end(), transparentTilemapLayers.begin(), transparentTilemapLayers.end());
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
        std::vector<Renderable> transparentUIPrimitives = primitiveRenderSystem->GetTransparentUIEntities();
        std::vector<Renderable> transparentUISprites = spriteRenderSystem->GetTransparentUIEntities();
        std::vector<Renderable> transparentUIModels = modelRenderSystem->GetTransparentUIEntities();
        std::vector<Renderable> transparentUIText = textRenderSystem->GetTransparentUIEntities();
        std::vector<Renderable> transparentUIEntites;
        transparentUIEntites.reserve(transparentUIPrimitives.size() + transparentUISprites.size() + transparentUIModels.size() + transparentUIText.size());
        transparentUIEntites.insert(transparentUIEntites.end(), transparentUIPrimitives.begin(), transparentUIPrimitives.end());
        transparentUIEntites.insert(transparentUIEntites.end(), transparentUISprites.begin(), transparentUISprites.end());
        transparentUIEntites.insert(transparentUIEntites.end(), transparentUIModels.begin(), transparentUIModels.end());
        transparentUIEntites.insert(transparentUIEntites.end(), transparentUIText.begin(), transparentUIText.end());
        DrawOrderedEntities(transparentUIEntites, cam);
    }

    //Draws a list of renderable entities that need to be sorted based on distance
    void DrawOrderedEntities(std::vector<Renderable> entities, Camera* cam)
    {
        //Calculate squared distances to camera
        for (Renderable& r : entities)
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
        for (const Renderable& r : entities)
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
