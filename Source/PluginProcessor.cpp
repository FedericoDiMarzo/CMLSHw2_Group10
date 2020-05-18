/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"


//==============================================================================
//==============================================================================
// GUI

/**
 * Namespace used for the parameters of the GUI.
 */
namespace IDs {
    static String intensity{"intensity"};
    static String mix{"mix"};
    static String blur{"blur"};
    static String rate{"rate"};
    static String enhance{"enhance"};
}

/**
 * Used to create the layout.
 */
AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
    AudioProcessorValueTreeState::ParameterLayout layout;


    // clustering the parameters into groups

    // central big knob
    auto big_knob = std::make_unique<AudioProcessorParameterGroup>("big_knob", TRANS ("BIG_KNOB"), "|");

    big_knob->addChild(
            std::make_unique<AudioParameterFloat>(IDs::intensity, "Intensity", NormalisableRange<float>(0.0f, 1.0f),
                                                  0.3f));


    // lfo group
    auto lfo = std::make_unique<AudioProcessorParameterGroup>("lfo", TRANS ("LFO"), "|");

    lfo->addChild(
            std::make_unique<AudioParameterFloat>(IDs::blur, "Blur", NormalisableRange<float>(0.0f, 1.0f), 0.0f),
            std::make_unique<AudioParameterFloat>(IDs::rate, "Rate", NormalisableRange<float>(0.0f, 1.0f), 0.3f));


    // utility group
    auto utility = std::make_unique<AudioProcessorParameterGroup>("utility", TRANS ("UTILITY"), "|");

    utility->addChild(
            std::make_unique<AudioParameterFloat>(IDs::enhance, "Enhance", NormalisableRange<float>(0.0f, 1.0f), 0.5f),
            std::make_unique<AudioParameterFloat>(IDs::mix, "Mix", NormalisableRange<float>(0.0f, 1.0f), 0.5f));


    layout.add(std::move(big_knob),
               std::move(lfo),
               std::move(utility));

    return layout;
}//end GUI Parameters definition

/**
 * Function triggered on parameter change.
 *
 * @param param identifier
 * @param value
 */
void Cmls_hw2_group10AudioProcessor::parameterChanged(const String &param, float value) {
    if (param.equalsIgnoreCase("intensity")) {
        chorus.setIntensity(value);
        chorus.setLfoDepth(value);
        chorus.setBlurFeedback(value);
    } else if (param.equalsIgnoreCase("mix")) {
        chorus.setWet(value);
    } else if (param.equalsIgnoreCase("blur")) {
        chorus.setBlurLevel(value);
        chorus.setBlurDelayTime(value);
    } else if (param.equalsIgnoreCase("rate")) {
        chorus.setLfoRate(value);
    } else if (param.equalsIgnoreCase("enhance")) {
        chorus.setStereoEnhance(value);
    } else {
        jassert(false); // no match
    }

}


AudioProcessorEditor *Cmls_hw2_group10AudioProcessor::createEditor() {
    return new foleys::MagicPluginEditor(magicState, BinaryData::scheme_xml, BinaryData::scheme_xmlSize);
}

bool Cmls_hw2_group10AudioProcessor::hasEditor() const {
    return true; // (change this to false if you choose to not supply an editor)
}


void Cmls_hw2_group10AudioProcessor::getStateInformation(MemoryBlock &destData) {
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    // magicState.getStateInformation(destData);

}

void Cmls_hw2_group10AudioProcessor::setStateInformation(const void *data, int sizeInBytes) {
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // magicState.setStateInformation(data, sizeInBytes, getActiveEditor());
}

//==============================================================================
//==============================================================================
// Constructor, destructor


Cmls_hw2_group10AudioProcessor::Cmls_hw2_group10AudioProcessor()

#ifndef JucePlugin_PreferredChannelConfigurations
        :
        AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                               .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                               .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
        ),
#endif


        //add Listener for the parameters
        treeState(*this, nullptr, "PARAMETERS", createParameterLayout()) {

    // adding listener
    treeState.addParameterListener(IDs::intensity, this);
    treeState.addParameterListener(IDs::blur, this);
    treeState.addParameterListener(IDs::rate, this);
    treeState.addParameterListener(IDs::mix, this);
    treeState.addParameterListener(IDs::enhance, this);

}


Cmls_hw2_group10AudioProcessor::~Cmls_hw2_group10AudioProcessor() {
}


//==============================================================================
//==============================================================================
// AudioProcessor main functions

void Cmls_hw2_group10AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {

    // just to be safe
    treeState.getParameter("mix")->setValue(0.5);
    treeState.getParameter("enhance")->setValue(0.5);
    treeState.getParameter("intensity")->setValue(0.3);
    treeState.getParameter("rate")->setValue(0.3);
    treeState.getParameter("blur")->setValue(0.0);


    //setting the initial state
    intensity = treeState.getParameterAsValue(IDs::intensity).getValue();
    chorus.setIntensity(intensity);
    chorus.setLfoDepth(intensity);
    chorus.setBlurFeedback(intensity);

    blur = treeState.getParameterAsValue(IDs::blur).getValue();
    chorus.setBlurLevel(blur);
    chorus.setBlurDelayTime(blur);

    rate = treeState.getParameterAsValue(IDs::rate).getValue();
    chorus.setLfoRate(rate);

    mix = treeState.getParameterAsValue(IDs::mix).getValue();
    chorus.setWet(mix);

    enhance = treeState.getParameterAsValue(IDs::enhance).getValue();
    chorus.setStereoEnhance(enhance);

    chorus.prepareToPlay(sampleRate, samplesPerBlock);
}


void Cmls_hw2_group10AudioProcessor::releaseResources() {
    chorus.releaseResources();
    //delay.releaseResources();
}

void Cmls_hw2_group10AudioProcessor::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) {
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());


    chorus.processBlock(buffer, midiMessages);
}

//==============================================================================
//==============================================================================


const String Cmls_hw2_group10AudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool Cmls_hw2_group10AudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Cmls_hw2_group10AudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Cmls_hw2_group10AudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Cmls_hw2_group10AudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int Cmls_hw2_group10AudioProcessor::getNumPrograms() {
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int Cmls_hw2_group10AudioProcessor::getCurrentProgram() {
    return 0;
}

void Cmls_hw2_group10AudioProcessor::setCurrentProgram(int index) {
}

const String Cmls_hw2_group10AudioProcessor::getProgramName(int index) {
    return {};
}

void Cmls_hw2_group10AudioProcessor::changeProgramName(int index, const String &newName) {
}


#ifndef JucePlugin_PreferredChannelConfigurations

bool Cmls_hw2_group10AudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const {
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() == AudioChannelSet::disabled()
        || layouts.getMainOutputChannelSet() == AudioChannelSet::disabled())
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


// This creates new instances of the plugin..
AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new Cmls_hw2_group10AudioProcessor();
}
