#include "PluginProcessor.h"
#include "PluginEditor.h"

EaPureCompressorAudioProcessor::EaPureCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
}

EaPureCompressorAudioProcessor::~EaPureCompressorAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
EaPureCompressorAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "threshold", "Threshold",
      juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -10.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "ratio", "Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
      2.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "attack", "Attack", juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f),
      10.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "release", "Release",
      juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "gain", "Gain", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));

  return {params.begin(), params.end()};
}

const juce::String EaPureCompressorAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool EaPureCompressorAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool EaPureCompressorAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool EaPureCompressorAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double EaPureCompressorAudioProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int EaPureCompressorAudioProcessor::getNumPrograms() { return 1; }

int EaPureCompressorAudioProcessor::getCurrentProgram() { return 0; }

void EaPureCompressorAudioProcessor::setCurrentProgram(int index) {}

const juce::String EaPureCompressorAudioProcessor::getProgramName(int index) {
  return {};
}

void EaPureCompressorAudioProcessor::changeProgramName(
    int index, const juce::String &newName) {}

void EaPureCompressorAudioProcessor::prepareToPlay(double sampleRate,
                                                   int samplesPerBlock) {
  compressor.prepare(sampleRate, samplesPerBlock);
  coreProtect.prepare(sampleRate, samplesPerBlock);
  saturation.prepare(sampleRate, samplesPerBlock);
}

void EaPureCompressorAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EaPureCompressorAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;

  return true;
}
#endif

void EaPureCompressorAudioProcessor::processBlock(
    juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Get parameters
  auto threshold = apvts.getRawParameterValue("threshold")->load();
  auto ratio = apvts.getRawParameterValue("ratio")->load();
  auto attack = apvts.getRawParameterValue("attack")->load();
  auto release = apvts.getRawParameterValue("release")->load();
  auto gain = apvts.getRawParameterValue("gain")->load();

  // 1. Core Protect (Dynamic Ratio Modulation)
  // CoreProtect analyzes the signal and returns a modified ratio
  float effectiveRatio = coreProtect.process(buffer, ratio);

  // 2. Base Engine (VCA Compression)
  compressor.process(buffer, threshold, effectiveRatio, attack, release);

  // 3. Crystalline Saturation & Output Gain
  saturation.process(buffer, gain);
}

bool EaPureCompressorAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor *EaPureCompressorAudioProcessor::createEditor() {
  return new EaPureCompressorAudioProcessorEditor(*this);
}

void EaPureCompressorAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void EaPureCompressorAudioProcessor::setStateInformation(const void *data,
                                                         int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new EaPureCompressorAudioProcessor();
}
