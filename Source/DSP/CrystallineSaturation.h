#pragma once
#include <JuceHeader.h>

class CrystallineSaturation {
public:
  CrystallineSaturation();
  void prepare(double sampleRate, int samplesPerBlock);

  // Process modifies the buffer in-place
  void process(juce::AudioBuffer<float> &buffer, float inputGainDB);

private:
  double sampleRate = 44100.0;
  juce::dsp::IIR::Filter<float> highPassFilter;
};
