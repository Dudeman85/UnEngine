#pragma once

#include "renderer/gl/Camera.h"
#include "renderer/gl/Utils.h"
#include "Color.h"

namespace une::renderer
{
    void Init();
    void UnifiedRenderPrepass();
    void UnifiedRenderPass(Camera* cam);

    //Draws a list of renderable entities that need to be sorted based on distance
    void DrawOrderedEntities(std::vector<Renderable> entities, Camera* cam);

    //Set the window clear color to given rgb(a) 0-255
    void SetBackgroundColor(Color c);
}
