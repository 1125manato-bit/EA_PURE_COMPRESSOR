#include "CompressorEngine.h"

CompressorEngine::CompressorEngine() {}

void CompressorEngine::prepare(double sr, int samplesPerBlock) {
  sampleRate = sr;
  envelope = 0.0f;
}

void CompressorEngine::process(juce::AudioBuffer<float> &buffer,
                               float threshold, float ratio, float attackMs,
                               float releaseMs) {
  auto numChannels = buffer.getNumChannels();
  auto numSamples = buffer.getNumSamples();

  // Simple VCA modeling
  // Check parameters to avoid division by zero
  if (ratio < 1.0f)
    ratio = 1.0f;

  float attackCoeff = std::exp(-1.0f / (attackMs * 0.001f * sampleRate));
  float releaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * sampleRate));

  for (int i = 0; i < numSamples; ++i) {
    // 1. Detect Max/RMS Level
    float inLevel = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch) {
      inLevel = std::max(inLevel, std::abs(buffer.getSample(ch, i)));
    }

    // 2. Envelope Follower
    if (inLevel > envelope)
      envelope = attackCoeff * envelope + (1.0f - attackCoeff) * inLevel;
    else
      envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * inLevel;

    // 3. Gain Calculation
    float envelopedB = juce::Decibels::gainToDecibels(envelope);
    float gainReductiondB = 0.0f;

    if (envelopedB > threshold) {
      gainReductiondB = (envelopedB - threshold) * (1.0f - 1.0f / ratio);
    }

    lastGainReductionDB.store(gainReductiondB);

    float gain = juce::Decibels::decibelsToGain(-gainReductiondB);

    // 4. Apply Gain
    for (int ch = 0; ch < numChannels; ++ch) {
      float currentSample = buffer.getSample(ch, i);
      buffer.setSample(ch, i, currentSample * gain);
    }
  }
}
