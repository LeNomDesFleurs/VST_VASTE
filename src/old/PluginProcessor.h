/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "filter.hpp"
//==============================================================================
/**
 */
struct Settings{
  float frequencycutoff{0}, Q{0};

};

Settings getSettings(juce::AudioProcessorValueTreeState& apvts);

class TestpluginAudioProcessor : public juce::AudioProcessor, 
public juce::ComboBox::Listener{
 public:
  //==============================================================================
  TestpluginAudioProcessor();
  ~TestpluginAudioProcessor() override;

  //==============================================================================
  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  //==============================================================================
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  //==============================================================================
  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  //==============================================================================
  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  //==============================================================================
  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
  juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};

  void comboBoxChanged(ComboBox* comboBox) override;

 private:

  juce::AudioPluginFormatManager pluginFormatManager;
  juce::ComboBox pluginComboBox;

  std::unique_ptr<AudioPluginInstance> hostedPlugin;

  Settings filterSettings;

  noi::Filter::Biquad lpf = noi::Filter::Biquad("LPF", 500, 0.707);
  //  std::array<noi::Filter::LPF, 2> lpf = {noi::Filter::LPF(8000), noi::Filter::LPF(8000)};
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestpluginAudioProcessor)
};
