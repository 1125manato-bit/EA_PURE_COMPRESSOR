#include "CoreProtect.h"

CoreProtect::CoreProtect() {}

void CoreProtect::prepare(double sr, int samplesPerBlock) {
  sampleRate = sr;
  // 300Hz - 3kHz Bandpass
  auto coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(
      sampleRate, 1000.0f, 1.5f); // Center 1kHz, Q 1.5 approx cover 300-3k
  bandpassFilter.coefficients = coefficients;
  bandpassFilter.reset();
  envelope = 0.0f;
}

float CoreProtect::process(const juce::AudioBuffer<float> &buffer,
                           float originalRatio) {
  // Analyze the block to detect energy in the "Core" band (300Hz-3kHz)
  // We create a copy to filter without affecting the original audio
  juce::AudioBuffer<float> tempBuffer;
  tempBuffer.makeCopyOf(buffer);

  juce::dsp::AudioBlock<float> block(tempBuffer);
  juce::dsp::ProcessContextReplacing<float> context(block);
  bandpassFilter.process(context);

  // Calculate RMS of the bandpassed signal
  float rms = tempBuffer.getRMSLevel(0, 0, tempBuffer.getNumSamples());
  if (tempBuffer.getNumChannels() > 1) {
    rms =
        std::max(rms, tempBuffer.getRMSLevel(1, 0, tempBuffer.getNumSamples()));
  }

  // Simple Envelope Follower for smooth modulation
  float attack = 0.1f;   // fast attack
  float release = 0.01f; // slow release

  // Normalize RMS roughly (0.0 - 1.0)
  // If there is significant energy, reduce ratio

  // Dynamic modulation logic:
  // If energy is high, Ratio is reduced.
  // Example: originalRatio = 4.0.
  // If Core Energy is max, effectiveRatio approaches 2.0 (or some factor).

  // Let's assume a threshold for "meaningful core energy"
  float coreEnergy = std::min(rms * 4.0f, 1.0f); // Boost sensitivity

  // Effective Ratio = OriginalRatio - (OriginalRatio - 1) * Factor * CoreEnergy
  // If CoreEnergy is 1.0 (max), Ratio is slightly reduced.
  // Let's make it subtle but noticeable.
  // Reduce compression by up to 50% on the ratio scale.

  float reductionFactor =
      0.3f * coreEnergy; // Max 30% reduction in ratio "intensity"
  float effectiveRatio =
      originalRatio - (originalRatio - 1.0f) * reductionFactor;

  return std::max(1.0f, effectiveRatio);
}
