#include "renderer/UnifiedRenderer.h"

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
        glDepthFunc(GL_LESS);

        primitiveRenderSystem->Init();
        spriteRenderSystem->Init();
    }

    void UnifiedRenderPrepass()
    {
        //Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        spriteRenderSystem->Prepass();
    }

    void UnifiedRenderPass(Camera* cam)
    {
		glEnable(GL_DEPTH_BUFFER_BIT);

        //TODO
        primitiveRenderSystem->Update(cam);
        modelRenderSystem->Update(cam);
        modelRenderSystem->DrawUIElements(cam); //This is a bandaid patch for UI models
        textRenderSystem->Update(cam);

        //First render all opaque world entities (non semi-transparent & non UI)
        spriteRenderSystem->DrawOpaqueWorldEntities(cam);

        //Then sort all semi-transparent world entities and render them

        glClear(GL_DEPTH_BUFFER_BIT);

        //Then render all opaque UI entities
        spriteRenderSystem->DrawOpaqueUIEntities(cam);

        //Then sort all semi-transparent UI entities and render them
    }

    //Set the window clear color to given rgb(a) 0-255
    void SetBackgroundColor(float r, float g, float b, float a)
    {
        glClearColor(r / 255, g / 255, b / 255, a / 255);
    }
}