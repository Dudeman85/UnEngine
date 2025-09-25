#pragma once

#include "renderer/gl/Camera.h"

namespace une::renderer
{
    void Init();
    void UnifiedRenderPrepass();
    void UnifiedRenderPass(Camera* cam);

    //Set the window clear color to given rgb(a) 0-255
    void SetBackgroundColor(float r, float g, float b, float a = 255);
}