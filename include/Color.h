#pragma once

#include "Vector.h"

namespace une
{
    class Color
    {
    public:
        explicit Color(int red, int green, int blue, int alpha = 255);
        explicit Color(float red, float green, float blue, float alpha = 1.0f);

        Color(const Vector4& c);
        static Color FromHSV(float hue, float saturation, float value);
        Color AsHSV() const;
        Color As8Bit() const;
        Color AsSRGB() const;

        bool isSRGB;
        float r, g, b, a;
    };
}
