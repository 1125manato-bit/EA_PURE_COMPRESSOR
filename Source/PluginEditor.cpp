#include "PluginEditor.h"
#include "PluginProcessor.h"

EaPureCompressorAudioProcessorEditor::EaPureCompressorAudioProcessorEditor(
    EaPureCompressorAudioProcessor &p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p) {
  auto bg = juce::ImageCache::getFromMemory(BinaryData::background_png,
                                            BinaryData::background_pngSize);
  setSize(bg.getWidth(), bg.getHeight());

  // Helper to setup sliders
  auto setupSlider = [this](juce::Slider &slider, juce::Label &label,
                            std::unique_ptr<SliderAttachment> &attachment,
                            const juce::String &name,
                            const juce::String &paramId) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    attachment = std::make_unique<SliderAttachment>(audioProcessor.apvts,
                                                    paramId, slider);
  };

  metalKnobLnf.setImage(juce::ImageCache::getFromMemory(
      BinaryData::knob_metal_png, BinaryData::knob_metal_pngSize));
  blackKnobLnf.setImage(juce::ImageCache::getFromMemory(
      BinaryData::knob_black_png, BinaryData::knob_black_pngSize));

  setupSlider(thresholdSlider, thresholdLabel, thresholdAtt, "Threshold",
              "threshold");
  thresholdSlider.setLookAndFeel(&metalKnobLnf);
  thresholdSlider.setRotaryParameters(-2.356f, 2.356f, true); // +/- 135 deg

  setupSlider(ratioSlider, ratioLabel, ratioAtt, "Ratio", "ratio");
  ratioSlider.setLookAndFeel(&blackKnobLnf);
  ratioSlider.setRotaryParameters(-2.356f, 2.356f, true);

  setupSlider(gainSlider, gainLabel, gainAtt, "Gain", "gain");
  gainSlider.setLookAndFeel(&metalKnobLnf);
  gainSlider.setRotaryParameters(-2.356f, 2.356f, true);

  setupSlider(attackSlider, attackLabel, attackAtt, "Attack", "attack");
  attackSlider.setLookAndFeel(&blackKnobLnf);
  attackSlider.setRotaryParameters(-2.356f, 2.356f, true);

  setupSlider(releaseSlider, releaseLabel, releaseAtt, "Release", "release");
  releaseSlider.setLookAndFeel(&blackKnobLnf);
  releaseSlider.setRotaryParameters(-2.356f, 2.356f, true);

  // Hide Labels as requested
  thresholdLabel.setVisible(false);
  ratioLabel.setVisible(false);
  gainLabel.setVisible(false);
  attackLabel.setVisible(false);
  releaseLabel.setVisible(false);

  // Disable interception if debug (hacky but works for drag)
  // Actually, Editor::mouseDown will only be called if sliders don't intercept.
  // So we must force them to not intercept in debug mode.
  bool intercept = !debugMode;
  thresholdSlider.setInterceptsMouseClicks(intercept, intercept);
  ratioSlider.setInterceptsMouseClicks(intercept, intercept);
  attackSlider.setInterceptsMouseClicks(intercept, intercept);
  releaseSlider.setInterceptsMouseClicks(intercept, intercept);
  gainSlider.setInterceptsMouseClicks(intercept, intercept);

  // Debug Label
  addAndMakeVisible(debugLabel);
  debugLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
  debugLabel.setColour(juce::Label::backgroundColourId,
                       juce::Colours::black.withAlpha(0.6f));

  startTimerHz(60); // Faster meter update (600Hz might be too much for UI, 60
                    // is standard smooth)
  setWantsKeyboardFocus(true);
}

EaPureCompressorAudioProcessorEditor::~EaPureCompressorAudioProcessorEditor() {
  thresholdSlider.setLookAndFeel(nullptr);
  ratioSlider.setLookAndFeel(nullptr);
  attackSlider.setLookAndFeel(nullptr);
  releaseSlider.setLookAndFeel(nullptr);
  gainSlider.setLookAndFeel(nullptr);
}

void EaPureCompressorAudioProcessorEditor::paint(juce::Graphics &g) {
  auto bg = juce::ImageCache::getFromMemory(BinaryData::background_png,
                                            BinaryData::background_pngSize);
  g.drawImageAt(bg, 0, 0);

  // Meter Needle logic
  auto w = getWidth();
  auto h = getHeight();

  // Meter Needle logic
  // Use member bound
  juce::Rectangle<int> meterArea = meterBounds;

  // Meter Drawing
  g.setColour(juce::Colours::red); // Red Needle
  float currentGR = audioProcessor.getGainReduction();
  float normalizedGR = juce::jlimit(0.0f, 1.0f, currentGR / 20.0f);
  float pivotX = meterArea.getCentreX();
  float pivotY = meterArea.getBottom() + 10;
  float needleLength = meterArea.getHeight() * 0.9f;

  // 130 degrees total sweep = +/- 65 degrees from center
  float maxAngle = juce::MathConstants<float>::pi * (65.0f / 180.0f);
  float angle = maxAngle - (normalizedGR * 2.0f * maxAngle);

  float endX = pivotX + std::sin(angle) * needleLength;
  float endY = pivotY - std::cos(angle) * needleLength;
  g.drawLine(pivotX, pivotY, endX, endY, 2.0f);

  // DEBUG OVERLAY
  if (debugMode) {
    g.setColour(juce::Colours::green);
    g.drawRect(meterArea, 2); // Meter Box

    g.setColour(juce::Colours::yellow);
    g.drawRect(thresholdSlider.getBounds(), 1);
    g.drawRect(ratioSlider.getBounds(), 1);
    g.drawRect(attackSlider.getBounds(), 1);
    g.drawRect(releaseSlider.getBounds(), 1);
    g.drawRect(gainSlider.getBounds(), 1);

    g.drawText("METER AREA", meterArea, juce::Justification::centred, false);
  }
}

void EaPureCompressorAudioProcessorEditor::mouseMove(
    const juce::MouseEvent &e) {
  if (debugMode) {
    debugLabel.setText(juce::String(e.x) + ", " + juce::String(e.y),
                       juce::dontSendNotification);
    debugLabel.setBounds(e.x + 10, e.y + 10, 100, 20);
  }
}

// 1. UI Resolution
void EaPureCompressorAudioProcessorEditor::resized() {
  // 1024 x 614
  auto bg = juce::ImageCache::getFromMemory(BinaryData::background_png,
                                            BinaryData::background_pngSize);
  if (bg.isValid()) {
    setSize(1024, 614);
  }

  // 2. Exact Coordinates (User Provided Dump)
  thresholdBounds = {94, 342, 228, 215};
  gainBounds = {714, 341, 208, 216};
  ratioBounds = {466, 317, 92, 112};
  attackBounds = {363, 435, 98, 112};
  releaseBounds = {565, 433, 94, 117};
  meterBounds = {390, 130, 245, 131};

  updateLayoutBounds();
}

void EaPureCompressorAudioProcessorEditor::updateLayoutBounds() {
  thresholdSlider.setBounds(thresholdBounds);
  ratioSlider.setBounds(ratioBounds);
  attackSlider.setBounds(attackBounds);
  releaseSlider.setBounds(releaseBounds);
  gainSlider.setBounds(gainBounds);

  // Labels - User Provided Coordinates
  thresholdLabel.setBounds(146, 538, 120, 24);
  gainLabel.setBounds(756, 538, 120, 24);
  ratioLabel.setBounds(472, 400, 80, 24);
  attackLabel.setBounds(370, 510, 80, 24);
  releaseLabel.setBounds(572, 510, 80, 24);
}
juce::Rectangle<int> *
EaPureCompressorAudioProcessorEditor::getBoundsForIndex(int index) {
  switch (index) {
  case 0:
    return &thresholdBounds;
  case 1:
    return &ratioBounds;
  case 2:
    return &attackBounds;
  case 3:
    return &releaseBounds;
  case 4:
    return &gainBounds;
  case 5:
    return &meterBounds;
  default:
    return nullptr;
  }
}

void EaPureCompressorAudioProcessorEditor::mouseDown(
    const juce::MouseEvent &e) {
  if (e.originalComponent == &debugLabel) {
    // Toggle debug mode disabled logic here if needed, but for now we just want
    // dragging
  }

  if (!debugMode)
    return;

  selectedIndex = -1;

  // Check components
  for (int i = 0; i < 6; ++i) {
    if (getBoundsForIndex(i)->contains(e.getPosition())) {
      selectedIndex = i;
      break;
    }
  }

  if (selectedIndex != -1) {
    initialDragBounds = *getBoundsForIndex(selectedIndex);
    dragStartPos = e.getPosition();
    isResizing = e.mods.isShiftDown(); // Shift to resize
  }
}

void EaPureCompressorAudioProcessorEditor::mouseDrag(
    const juce::MouseEvent &e) {
  if (!debugMode || selectedIndex == -1)
    return;

  auto *bounds = getBoundsForIndex(selectedIndex);
  auto diff = e.getPosition() - dragStartPos;

  if (isResizing) {
    bounds->setWidth(juce::jmax(10, initialDragBounds.getWidth() + diff.x));
    bounds->setHeight(juce::jmax(10, initialDragBounds.getHeight() + diff.y));
  } else {
    bounds->setPosition(initialDragBounds.getPosition() + diff);
  }

  updateLayoutBounds();
  repaint();

  // Update debug text with code snippet
  debugLabel.setText(juce::String::formatted("Bounds: %d, %d, %d, %d",
                                             bounds->getX(), bounds->getY(),
                                             bounds->getWidth(),
                                             bounds->getHeight()),
                     juce::dontSendNotification);
}

void EaPureCompressorAudioProcessorEditor::mouseUp(const juce::MouseEvent &) {
  selectedIndex = -1;
}

bool EaPureCompressorAudioProcessorEditor::keyPressed(
    const juce::KeyPress &key) {
  if (debugMode && key.getKeyCode() == 'S') {
    juce::File desktop =
        juce::File::getSpecialLocation(juce::File::userDesktopDirectory);
    juce::File dumpFile = desktop.getChildFile("EA_PURE_COMPRESSOR_LAYOUT.txt");

    juce::String layoutLog;
    layoutLog += "// --- LAYOUT DUMP ---\n";

    auto appendBounds = [&](const juce::String &name, juce::Component &c) {
      auto b = c.getBounds();
      layoutLog += name + "Bounds = { " + juce::String(b.getX()) + ", " +
                   juce::String(b.getY()) + ", " + juce::String(b.getWidth()) +
                   ", " + juce::String(b.getHeight()) + " };\n";
    };

    appendBounds("threshold", thresholdSlider);
    appendBounds("gain", gainSlider);
    appendBounds("ratio", ratioSlider);
    appendBounds("attack", attackSlider);
    appendBounds("release", releaseSlider);

    // Meter (use member)
    layoutLog += "meterBounds = { " + juce::String(meterBounds.getX()) + ", " +
                 juce::String(meterBounds.getY()) + ", " +
                 juce::String(meterBounds.getWidth()) + ", " +
                 juce::String(meterBounds.getHeight()) + " };\n";

    layoutLog += "\n// --- LABELS ---\n";
    appendBounds("thresholdLabel", thresholdLabel);
    appendBounds("gainLabel", gainLabel);
    appendBounds("ratioLabel", ratioLabel);
    appendBounds("attackLabel", attackLabel);
    appendBounds("releaseLabel", releaseLabel);

    dumpFile.replaceWithText(layoutLog);

    debugLabel.setText("Saved to Desktop!", juce::dontSendNotification);
    return true;
  }
  return false;
}

void EaPureCompressorAudioProcessorEditor::timerCallback() {
  // repaint to update meter if we were pulling values
  // getGR() implementation pending in Processor
  repaint();
}
