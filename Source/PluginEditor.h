#pragma once

#include "KnobLookAndFeel.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

class EaPureCompressorAudioProcessorEditor : public juce::AudioProcessorEditor,
                                             public juce::Timer {
public:
  EaPureCompressorAudioProcessorEditor(EaPureCompressorAudioProcessor &);
  ~EaPureCompressorAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;
  void timerCallback() override;
  void mouseMove(const juce::MouseEvent &e) override;

private:
  EaPureCompressorAudioProcessor &audioProcessor;

  // Attachments
  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

  juce::Slider thresholdSlider, ratioSlider, attackSlider, releaseSlider,
      gainSlider;
  std::unique_ptr<SliderAttachment> thresholdAtt, ratioAtt, attackAtt,
      releaseAtt, gainAtt;

  juce::Label thresholdLabel, ratioLabel, attackLabel, releaseLabel, gainLabel;

  KnobLookAndFeel metalKnobLnf;
  KnobLookAndFeel blackKnobLnf;

  // Debug
  bool debugMode = false;
  juce::Label debugLabel;

  // Interactive Layout Helpers
  int selectedIndex = -1;
  juce::Point<int> dragStartPos;
  juce::Rectangle<int> initialDragBounds;
  bool isResizing = false;

  // Component Layout Positions
  juce::Rectangle<int> thresholdBounds, ratioBounds, attackBounds,
      releaseBounds, gainBounds, meterBounds;

  void updateLayoutBounds();
  juce::Rectangle<int> *getBoundsForIndex(int index);

  void mouseDown(const juce::MouseEvent &e) override;
  void mouseDrag(const juce::MouseEvent &e) override;
  void mouseUp(const juce::MouseEvent &e) override;
  bool keyPressed(const juce::KeyPress &key) override;

  // Metering
  float grLevel = 0.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
      EaPureCompressorAudioProcessorEditor)
};
