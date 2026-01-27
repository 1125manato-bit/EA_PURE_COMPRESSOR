#pragma once
#include <JuceHeader.h>

class KnobLookAndFeel : public juce::LookAndFeel_V4 {
public:
  KnobLookAndFeel() {}

  void setImage(const juce::Image &img) { knobImage = img; }

  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider &slider) override {
    if (knobImage.isValid()) {
      const float angle =
          rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

      // Calculate scale to fit
      float scale = std::min((float)width / knobImage.getWidth(),
                             (float)height / knobImage.getHeight());

      auto transform =
          juce::AffineTransform::rotation(angle, knobImage.getWidth() / 2.0f,
                                          knobImage.getHeight() / 2.0f)
              .scaled(scale)
              .translated(x + (width - knobImage.getWidth() * scale) / 2.0f,
                          y + (height - knobImage.getHeight() * scale) / 2.0f);

      g.drawImageTransformed(knobImage, transform);
    } else {
      // Fallback
      juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPos,
                                             rotaryStartAngle, rotaryEndAngle,
                                             slider);
    }
  }

private:
  juce::Image knobImage;
};
