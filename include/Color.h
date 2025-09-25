#pragma once

#include "Vector.h"

namespace une
{
    class Color
    {
    public:
        explicit Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
        explicit Color(float red, float green, float blue, float alpha = 1.0f);

        //TODO: implementations
        Color(Vector4 c); //TODO also reverse to Vector4
        static Color FromHSB(float hue, float saturation, float brightness);
        Color As8Bit() const;
        Color AsSRGB() const;

        bool isSRGB;
        float r, g, b, a;
    };
}
