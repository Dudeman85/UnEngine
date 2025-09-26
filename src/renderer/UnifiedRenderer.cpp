#include "renderer/UnifiedRenderer.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "UnEngine.h"

namespace une::renderer
{
    void Init()
    {
        //Set the screen clear color to black
        glClearColor(0, 0, 0, 1.0f);

        //Enable transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        //Enable Depth buffering
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        primitiveRenderSystem->Init();
        spriteRenderSystem->Init();
    }

    void UnifiedRenderPrepass()
    {
        //Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        spriteRenderSystem->Prepass();
        primitiveRenderSystem->Prepass();
    }

    void UnifiedRenderPass(Camera* cam)
    {
		glEnable(GL_DEPTH_BUFFER_BIT);

        //TODO
        modelRenderSystem->Update(cam);
        modelRenderSystem->DrawUIElements(cam); //This is a bandaid patch for UI models
        textRenderSystem->Update(cam);


        //First render all opaque world entities (non semi-transparent & non UI)
        spriteRenderSystem->DrawOpaqueWorldEntities(cam);
        primitiveRenderSystem->DrawOpaqueWorldEntities(cam);

        //Then sort all semi-transparent world entities and render them
        std::vector<Renderable> transparentSprites = spriteRenderSystem->GetTransparentWorldEntities();
        std::vector<Renderable> transparentPrimitives = primitiveRenderSystem->GetTransparentWorldEntities();
        std::vector<Renderable> transparentEntites;
        transparentEntites.reserve(transparentSprites.size() + transparentPrimitives.size());
        transparentEntites.insert(transparentEntites.end(), transparentSprites.begin(), transparentSprites.end());
        transparentEntites.insert(transparentEntites.end(), transparentPrimitives.begin(), transparentPrimitives.end());
        DrawOrderedEntities(transparentEntites, cam);

        //Clear depth buffer to always render UI above world
        glClear(GL_DEPTH_BUFFER_BIT);

        //Then render all opaque UI entities
        spriteRenderSystem->DrawOpaqueUIEntities(cam);
        primitiveRenderSystem->DrawOpaqueUIEntities(cam);

        //Then sort all semi-transparent UI entities and render them
        std::vector<Renderable> transparentUISprites = spriteRenderSystem->GetTransparentUIEntities();
        std::vector<Renderable> transparentUIPrimitives = primitiveRenderSystem->GetTransparentUIEntities();
        std::vector<Renderable> transparentUIEntites;
        transparentUIEntites.reserve(transparentUISprites.size() + transparentUIPrimitives.size());
        transparentUIEntites.insert(transparentUIEntites.end(), transparentUISprites.begin(), transparentUISprites.end());
        transparentUIEntites.insert(transparentUIEntites.end(), transparentUIPrimitives.begin(), transparentUIPrimitives.end());
        DrawOrderedEntities(transparentUIEntites, cam);
    }

    //Draws a list of renderable entities that need to be sorted based on distance
    void DrawOrderedEntities(std::vector<Renderable> entities, Camera* cam)
    {
        //Calculate squared distances to camera
        for (Renderable& r : entities)
        {
            Vector3 pos = TransformSystem::GetGlobalTransform(r.entity).position;
            //TODO: This needs to take into account camera rotation
            r.distToCamera = cam->position.z - pos.z;
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
            r.render(r.entity, cam);
        }
    }

    //Set the window clear color
    void SetBackgroundColor(Color c)
    {
        Color srgb = c.AsSRGB();
        glClearColor(srgb.r, srgb.g, srgb.b, srgb.a);
    }
}
