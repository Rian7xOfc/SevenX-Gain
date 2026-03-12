#pragma once
#include <JuceHeader.h>
#include <cmath>

class SevenXSaturation
{
public:

    static float processSample(float input, bool active, bool isMasterMode, float drive = 1.6f, float mix = 0.7f)
    {
        if (!active)
            return input;

        float dry = input;
        float x;
        float sat;

        if (isMasterMode)
        {
            // Saturação transparente
            float masterDrive = 1.15f;
            x = input * masterDrive;

            // curva suave
            sat = x / (1.0f + std::fabs(x));

            float masterMix = 0.4f;

            float out = (dry * (1.0f - masterMix)) + (sat * masterMix);

            return juce::jlimit(-1.0f, 1.0f, out);
        }
        else
        {
            // Saturação estilo válvula
            x = input * drive;

            if (x > 0.0f)
                sat = (1.5f * x) / (1.0f + std::fabs(x));
            else
                sat = (1.2f * x) / (1.0f + std::fabs(x));

            sat = std::tanh(sat);

            sat *= 0.85f;

            float out = (dry * (1.0f - mix)) + (sat * mix);

            return juce::jlimit(-1.0f, 1.0f, out);
        }
    }
};