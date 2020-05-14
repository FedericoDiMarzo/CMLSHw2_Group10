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
#include "MorphingLfo.h"
#include "ProcessorTemplate.h"

#define NUMBER_OF_DELAYS 2

class Chorus : public ProcessorTemplate {
public:
    Chorus();
    virtual ~Chorus();


    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;


private:
    std::vector<std::unique_ptr<Delay>> delayLines;
    std::vector<std::unique_ptr<AudioSampleBuffer>> bufferPool;
    std::vector<std::unique_ptr<MorphingLfo>> lfoPool;
    int lfoSubRate = 100;
    float wet = 0.8;
    float feedback = 0;
    float sampleRate = 0;
};