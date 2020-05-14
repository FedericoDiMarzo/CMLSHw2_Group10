/*
  ==============================================================================

    ProcessorTemplate.h
    Created: 14 May 2020 7:11:14pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ProcessorTemplate {
public:
    ProcessorTemplate() = default;
    virtual ~ProcessorTemplate() = default;

    //==============================================================================
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock) = 0;
    virtual void releaseResources() = 0;
    virtual void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) = 0;
};