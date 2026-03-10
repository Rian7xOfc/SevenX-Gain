#pragma once
#include <JuceHeader.h>

/**
 * SevenXSaturation PRO
 * Saturação otimizada para vocais (estilo válvula analógica)
 */

class SevenXSaturation
{
public:

    static float processSample(float input, bool active, float drive = 1.4f, float mix = 1.0f)
    {
        if (!active)
            return input;

        float dry = input;

        // Drive interno
        float x = input * drive;

        // Saturação assimétrica estilo válvula
        float saturated;

        if (x >= 0.0f)
            saturated = std::tanh(x);
        else
            saturated = std::tanh(x * 0.85f);

        // Normalização leve
        saturated *= 0.9f;

        // Mix Dry/Wet
        return (dry * (1.0f - mix)) + (saturated * mix);
    }
};