#include "Color.h"

#include <algorithm>

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

    Color Color::FromHSV(float hue, float saturation, float value)
    {
        float r = 0, g = 0, b = 0;
        int i = floor(hue * 6);
        float f = hue * 6 - i;
        float p = value * (1 - saturation);
        float q = value * (1 - f * saturation);
        float t = value * (1 - (1 - f) * saturation);

        switch(i % 6)
        {
            case 0: r = value, g = t, b = p; break;
            case 1: r = q, g = value, b = p; break;
            case 2: r = p, g = value, b = t; break;
            case 3: r = p, g = q, b = value; break;
            case 4: r = t, g = p, b = value; break;
            case 5: r = value, g = p, b = q; break;
        }

        return Color(r, g, b);
    }

    Color Color::AsHSV() const
    {
        Color srgb = AsSRGB();

        float v = std::max(srgb.r, std::max(srgb.g, srgb.b));
        float d = v - std::min(srgb.r, std::min(srgb.g, srgb.b));

        if (d == 0)
        {
            //Grey
            return Color(0.f, 0.f, v, srgb.a);
        }
        else
        {
            float s = d / v;

            float h = 0;
            float dr = (((v - srgb.r) / 6) + (d / 2)) / d;
            float dg = (((v - srgb.g) / 6) + (d / 2)) / d;
            float db = (((v - srgb.b) / 6) + (d / 2)) / d;
            if (srgb.r == v)
                h = db - dg;
            else if (srgb.g == v)
                h = (1/3) + dr - db;
            else
                h = (2/3) + dg - dr;

            return Color(h, s, v, srgb.a);
        }
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

    std::string Color::ToString() const
    {
        char buff[1000];
        if (isSRGB)
            std::snprintf(buff, sizeof(buff), "Color(%f, %f, %f, %f)", r, g, b, a);
        else
            std::snprintf(buff, sizeof(buff), "Color(%d, %d, %d, %d)", (int)r, (int)g, (int)b, (int)a);

        return std::string(buff);
    }
}