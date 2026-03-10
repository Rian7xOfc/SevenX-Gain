#pragma once
#include <JuceHeader.h>

/**
 * SevenXSaturation - Algoritmo otimizado para Vocais.
 * Adiciona harmônicos quentes e controla os picos de forma suave.
 */
class SevenXSaturation
{
public:
    /**
     * Processa a amostra de áudio com uma saturação suave.
     * @param entrada A amostra original.
     * @param estaAtivo Se o efeito deve ser aplicado.
     * @return A amostra processada.
     */
    static float processSample (float entrada, bool estaAtivo)
    {
        if (!estaAtivo)
            return entrada;

        // Otimização para Voz: Saturação assimétrica.
        // Isso imita o comportamento de válvulas, que soam muito bem em vozes.
        float x = entrada * 1.3f; // Aumentamos um pouco o 'drive' interno
        
        if (x > 0.0f)
            return std::tanh(x); // Compressão suave para picos positivos
        else
            return std::tanh(x * 0.9f); // Curva levemente diferente para os picos negativos
    }
};