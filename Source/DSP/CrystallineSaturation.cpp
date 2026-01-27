#include "CrystallineSaturation.h"

CrystallineSaturation::CrystallineSaturation() {}

void CrystallineSaturation::prepare(double sr, int samplesPerBlock) {
  sampleRate = sr;
  // Highpass at 15kHz to isolate "Air" band
  auto coefficients =
      juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 15000.0f);
  highPassFilter.coefficients = coefficients;
  highPassFilter.reset();
}

void CrystallineSaturation::process(juce::AudioBuffer<float> &buffer,
                                    float inputGainDB) {
  // Crystalline Saturation:
  // 1. High-shelf boost or high-frequency harmonic generation linked to Gain.
  // 2. Here we implement a parallel saturation path for >15kHz.

  // Create a copy for high frequency extraction
  juce::AudioBuffer<float> highFreqBuffer;
  highFreqBuffer.makeCopyOf(buffer);

  juce::dsp::AudioBlock<float> block(highFreqBuffer);
  juce::dsp::ProcessContextReplacing<float> context(block);
  highPassFilter.process(context);

  float gainLinear = juce::Decibels::decibelsToGain(inputGainDB);

  // Apply saturation to the high frequencies
  // Simple soft clipper or even harmonic generator
  auto numSamples = highFreqBuffer.getNumSamples();
  auto numChannels = highFreqBuffer.getNumChannels();

  for (int ch = 0; ch < numChannels; ++ch) {
    auto *data = highFreqBuffer.getWritePointer(ch);
    const auto *origData = buffer.getReadPointer(ch);
    auto *outData = buffer.getWritePointer(ch);

    for (int i = 0; i < numSamples; ++i) {
      float x = data[i];
      // Even harmonic generation: x + a * x^2
      // We want to add "sparkle" so we rectify slightly
      float saturated = x + 0.5f * x * x;

      // Mix back into original signal
      // The amount of saturation is proportional to the Gain parameter
      // If Gain is high, we add more "Air"
      float mixAmount =
          0.1f * std::max(0.0f, inputGainDB / 24.0f); // Max 10% mix at max gain

      // Output = Original * Gain + SaturatedHighs * Mix
      outData[i] = origData[i] * gainLinear + saturated * mixAmount;
    }
  }
}
