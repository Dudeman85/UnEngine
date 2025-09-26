#include "Color.h"

namespace une
{
    Color::Color(int red, int green, int blue, int alpha)
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