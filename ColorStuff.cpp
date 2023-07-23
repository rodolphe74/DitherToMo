#include "ColorStuff.h"

string getPaletteKey(Color c)
{
    sprintf(key, "%d,%d,%d", c.quantumRed(), c.quantumGreen(), c.quantumBlue());
    string s(key);
    return s;
}

float correctedColorDelta(Color c1, Color c2)
{
    float dr = std::abs(c1.quantumRed() - c2.quantumRed());
    float dg = std::abs(c1.quantumGreen() - c2.quantumGreen());
    float db = std::abs(c1.quantumBlue() - c2.quantumBlue());
    return sqrt(30.0f * (dr * dr) + 59.0f * (dg * dg) + 11.0f * (db * db));
}

float preciseColorDelta(Color c1, Color c2)
{
    float dr = std::abs(c1.quantumRed() - c2.quantumRed());
    float dg = std::abs(c1.quantumGreen() - c2.quantumGreen());
    float db = std::abs(c1.quantumBlue() - c2.quantumBlue());
    return sqrt((dr * dr) + (dg * dg) + (db * db));
}

Color findPreciseClosestColorFromPalette(Color color, const map<string, PALETTE_ENTRY> &palette)
{
    float distance;
    float diff = std::numeric_limits<float>::max();
    Color nearest;

    for (auto p = palette.begin(); p != palette.end(); p++) {
        // Color current = p->second.color;
        distance = preciseColorDelta(p->second.color, color);
        if (distance < diff) {
            nearest = p->second.color;
            diff = distance;
        }
    }

    return nearest;
}



Color findCorrectedClosestColorFromPalette(Color color, const map<string, PALETTE_ENTRY> &palette)
{
    float distance;
    float diff = std::numeric_limits<float>::max();
    Color nearest;

    for (auto p = palette.begin(); p != palette.end(); p++) {
        // Color current = p->second.color;
        distance = correctedColorDelta(p->second.color, color);
        if (distance < diff) {
            nearest = p->second.color;
            diff = distance;
        }
    }

    return nearest;
}
