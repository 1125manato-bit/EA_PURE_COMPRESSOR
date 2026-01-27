#pragma once
#include <JuceHeader.h>

class CoreProtect {
public:
  CoreProtect();
  void prepare(double sampleRate, int samplesPerBlock);

  // Returns the modified ratio
  float process(const juce::AudioBuffer<float> &buffer, float originalRatio);

private:
  double sampleRate = 44100.0;
  juce::dsp::IIR::Filter<float> bandpassFilter;
  float envelope = 0.0f;
};
