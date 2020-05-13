/*
  ==============================================================================

    Chorus.h
    Created: 12 May 2020 11:59:22pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Delay.h"

#define NUMBER_OF_DELAYS 2

class Chorus : public AudioProcessor{
public:
    Chorus();
    virtual ~Chorus();


    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override          { return nullptr; }
    bool hasEditor() const override                        { return false; }

    //==============================================================================
    const String getName() const override                  { return {}; }
    bool acceptsMidi() const override                      { return false; }
    bool producesMidi() const override                     { return false; }
    double getTailLengthSeconds() const override           { return 0; }

    //==============================================================================
    int getNumPrograms() override                          { return 0; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const String getProgramName (int) override             { return "Chorus"; }
    void changeProgramName (int, const String&) override   {}

    //==============================================================================
    void getStateInformation (MemoryBlock&) override       {}
    void setStateInformation (const void*, int) override   {}

private:
    std::vector<Delay*> delayLines;
    std::vector<AudioSampleBuffer*> parallelBuffers;
    float wet = 0.8;
    float feedback = 0.15;
    float sampleRate = 0;
};