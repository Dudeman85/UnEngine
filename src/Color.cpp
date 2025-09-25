#include "Color.h"

namespace une
{
    Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        isSRGB = false;
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
    Color::Color(float red, float green, float blue, float alpha)
    {
        isSRGB = true;
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }

    Color Color::AsSRGB() const
    {
        if (isSRGB)
            return Color(r, g, b, a);
        else
            return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }
}