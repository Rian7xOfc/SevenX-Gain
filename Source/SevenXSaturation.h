#pragma once
#include <JuceHeader.h>

/**
 * SevenXSaturation PRO - Multi-Modo
 * Agora com suporte para processamento de Voz (Analógico) e Master (Transparente).
 */
class SevenXSaturation
{
public:
    /**
     * Processa a amostra com base no modo selecionado.
     * @param input Amostra original.
     * @param active Se o efeito está ligado.
     * @param isMasterMode True para modo Master, False para modo Voz.
     * @param drive Intensidade da saturação.
     * @param mix Balanço Dry/Wet.
     */
    static float processSample(float input, bool active, bool isMasterMode, float drive = 1.6f, float mix = 0.7f)
    {
        if (!active)
            return input;

        float dry = input;
        float x;
        float sat;

        if (isMasterMode)
        {
            // --- MODO MASTER (Saturação Simétrica / Transparente) ---
            // Drive mais baixo para preservar a dinâmica do mix inteiro
            float masterDrive = 1.15f; 
            x = input * masterDrive;
            
            // Saturação simétrica: afeta picos positivos e negativos igualmente
            // Isso gera harmônicos ímpares, que dão "peso" e "cola" ao som
            sat = std::tanh(x);
            
            // Mix padrão mais baixo para o Master ser sutil
            float masterMix = 0.4f; 
            return (dry * (1.0f - masterMix)) + (sat * masterMix);
        }
        else
        {
            // --- MODO VOZ (Saturação Assimétrica / Estilo Válvula) ---
            x = input * drive;

            if (x > 0.0f)
                sat = (1.5f * x) / (1.0f + std::abs(x));
            else
                sat = (1.2f * x) / (1.0f + std::abs(x));

            sat = std::tanh(sat);
            sat *= 0.85f; // Maquiagem de ganho

            return (dry * (1.0f - mix)) + (sat * mix);
        }
    }
};