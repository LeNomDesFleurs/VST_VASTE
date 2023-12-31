/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

#include "Component/XyPad.h"

struct CustomRotarySlider : juce::Slider{
  CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, 
  juce::Slider::TextEntryBoxPosition::NoTextBox){}
};

//==============================================================================
/**
 */
class TestpluginAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Button::Listener{
 public:
  TestpluginAudioProcessorEditor(TestpluginAudioProcessor &);
  ~TestpluginAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.

   juce::Label testlabel;

   TestpluginAudioProcessor &audioProcessor;
   std::unique_ptr<juce::Drawable> svgimg;
   Gui::XyPad xyPad;

   CustomRotarySlider frequencyCutoffKnob, QKnob;

   std::vector<juce::Component *> getComps();

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestpluginAudioProcessorEditor)
};
