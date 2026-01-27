#pragma once
#include <JuceHeader.h>

class CompressorEngine {
public:
  CompressorEngine();
  void prepare(double sampleRate, int samplesPerBlock);
  void process(juce::AudioBuffer<float> &buffer, float threshold, float ratio,
               float attackMs, float releaseMs);
  float getGainReductionDB() const { return lastGainReductionDB.load(); }

private:
  std::atomic<float> lastGainReductionDB{0.0f};
  double sampleRate = 44100.0;
  float envelope = 0.0f;
};
