/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Chorus.h"


//==============================================================================

class Cmls_hw2_group10AudioProcessor : public AudioProcessor, private AudioProcessorValueTreeState::Listener {
public:
    //==============================================================================
    Cmls_hw2_group10AudioProcessor();
    ~Cmls_hw2_group10AudioProcessor();

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
#endif

    //callback function for the parameters
    void parameterChanged(const String &param, float value) override;

    void processBlock(AudioBuffer<float> &, MidiBuffer &) override;

    //==============================================================================
    AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String &newName) override;

    //==============================================================================
    void getStateInformation(MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;


    //==============================================================================


private:
    Chorus chorus;

    // GUI parametes
    float intensity = 0.5f;
    float rate = 0.5f;
    float depth = 0.5f;
    float mix = 0.5f;
    float enhance = 0.5f;

    // GUI Editor
    AudioProcessorValueTreeState treeState;

    // MAGIC GUI: add this docking station for the GUI
    foleys::MagicProcessorState magicState{*this, treeState};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Cmls_hw2_group10AudioProcessor)
};
