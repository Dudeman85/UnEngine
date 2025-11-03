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
    Color::Color(const Vector4& c)
    {
        isSRGB = true;
        r = (float)c.x;
        g = (float)c.y;
        b = (float)c.z;
        a = (float)c.w;
    }

    Color Color::As8Bit() const
    {
        if (isSRGB)
            return Color(std::clamp(std::round(r * 255), 0.f, 255.f),
                std::clamp(std::round(g * 255), 0.f, 255.f),
                std::clamp(std::round(b * 255), 0.f, 255.f),
                std::clamp(std::round(a * 255), 0.f, 255.f));
        else
            return Color(r, g, b, a);
    }
    Color Color::AsSRGB() const
    {
        if (isSRGB)
            return Color(r, g, b, a);
        else
            return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
    }
}