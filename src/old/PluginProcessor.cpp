/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"


//==============================================================================
TestpluginAudioProcessor::TestpluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{

  appProperties.setStorageParameters ([&]
        {
            PropertiesFile::Options opt;
            opt.applicationName = getName();
            opt.commonToAllUsers = false;
            opt.doNotSave = false;
            opt.filenameSuffix = ".props";
            opt.ignoreCaseOfKeyNames = false;
            opt.storageFormat = PropertiesFile::StorageFormat::storeAsXML;
            opt.osxLibrarySubFolder = "Application Support";
            return opt;
        }());

        pluginFormatManager.addDefaultFormats();

        if (auto savedPluginList = appProperties.getUserSettings()->getXmlValue ("pluginList"))
            pluginList.recreateFromXml (*savedPluginList);

        MessageManagerLock lock;
        pluginList.addChangeListener (this);

        
  // Add supported plugin formats (VST, AU, etc.) to the format manager
    pluginFormatManager.addDefaultFormats();

    // Scan for available plugins
    juce::KnownPluginList knownPlugins;


    // Add VST3 format to the format manager
    // pluginFormatManager.addFormat(new VST3PluginFormat());

    // Locate the path to the VST3 plugin on your computer
    juce::File vst3PluginFile ("/Library/Audio/Plug-ins/VST3/Convex.vst3"); // Replace with the actual path

    // Load the VST3 plugin
    hostedPlugin = pluginFormatManager.createPluginInstance(vst3PluginFile, 48000, 128, callback);

    // TODOO path harcoded for now
    // pluginFormatManager.scanAndAddFileOrDirectoryForPlugins(knownPlugins, File("/Library/Audio/Plug-ins/VST3/"), true, true);

    // hostedPlugin->showWindow(PluginWindow::Type::normal);

    // Populate a menu with available plugins
    // for (int i = 0; i < knownPlugins.getNumTypes(); ++i)
    // {
    //     const PluginDescription& desc = knownPlugins.getType(i);
    //     pluginComboBox.addItem(desc.name, i + 1);
    // }

    // pluginComboBox.addListener(this);
}

TestpluginAudioProcessor::~TestpluginAudioProcessor() {}

//==============================================================================
const juce::String TestpluginAudioProcessor::getName() const {
  return JucePlugin_Name;
}

bool TestpluginAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool TestpluginAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool TestpluginAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double TestpluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int TestpluginAudioProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int TestpluginAudioProcessor::getCurrentProgram() { return 0; }

void TestpluginAudioProcessor::setCurrentProgram(int index) {}

const juce::String TestpluginAudioProcessor::getProgramName(int index) {
  return {};
}

void TestpluginAudioProcessor::changeProgramName(int index,
                                                 const juce::String &newName) {}

//==============================================================================
void TestpluginAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock) {
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
}

void TestpluginAudioProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TestpluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void TestpluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  filterSettings = getSettings(apvts);
  lpf.setParam(filterSettings.frequencycutoff, filterSettings.Q);

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  // Make sure to reset the state if your inner loop is processing
  // the samples and the outer loop is handling the channels.
  // Alternatively, you can process the samples with the channels
  // interleaved by keeping the same state.

  // for (int channel = 0; channel < totalNumInputChannels; ++channel) {
  //   auto *channelSamples = buffer.getWritePointer(channel);
  //      // for each sample in the channel
  //       for (auto n = 0; n < buffer.getNumSamples(); ++n) {
  //           const auto input = channelSamples[n];
  //           if (channel == 0){ 
  //           channelSamples[n] = lpf.process(input);
  //       }
  //       }
  // }


      if (hostedPlugin != nullptr)
    {
        // Process audio using the hosted plugin
        hostedPlugin->processBlock(buffer, midiMessages);
    }
}

Settings getSettings(juce::AudioProcessorValueTreeState& apvts){
  Settings settings;

  settings.frequencycutoff = apvts.getRawParameterValue("FrequencyCutoff")->load();
  settings.Q = apvts.getRawParameterValue("Q")->load();

  return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout TestpluginAudioProcessor::createParameterLayout(){

juce::AudioProcessorValueTreeState::ParameterLayout layout;

layout.add(std::make_unique<juce::AudioParameterFloat>("FrequencyCutoff", "FrequencyCutoff",
juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f), 500.f));
layout.add(std::make_unique<juce::AudioParameterFloat>("Q", "Q", juce::NormalisableRange<float>(0.5, 20.f, 1.f, 1.f), 0.707));

return layout;
}


//==============================================================================
bool TestpluginAudioProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *TestpluginAudioProcessor::createEditor() {
  return new TestpluginAudioProcessorEditor(*this);
  //return new GenericAudioProcessorEditor(*this);
}

//==============================================================================
void TestpluginAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void TestpluginAudioProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  // You should use this method to restore your parameters from this memory
  // block, whose contents will have been created by the getStateInformation()
  // call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new TestpluginAudioProcessor();
}


void CustomPluginProcessor::comboBoxChanged(ComboBox* comboBox)
{
    if (comboBox == &pluginComboBox)
    {
        int selectedPluginIndex = pluginComboBox.getSelectedItemIndex();

        if (selectedPluginIndex >= 0)
        {
            const PluginDescription& desc = knownPlugins.getType(selectedPluginIndex);
            AudioPluginInstance* pluginInstance = pluginFormatManager.createPluginInstance(desc);

            if (pluginInstance != nullptr)
            {
                // Store the plugin instance for later use
                hostedPlugin.reset(pluginInstance);
            }
        }
    }
}