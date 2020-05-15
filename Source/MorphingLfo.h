/*
  ==============================================================================

    MorphingLfo.h
    Created: 14 May 2020 9:45:39pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessorTemplate.h"

class MorphingLfo : public ProcessorTemplate {
public:
    MorphingLfo(int morphingSteps, int tableResolution);
    virtual ~MorphingLfo();

    void setFrequency(float frequency);

    float getFrequency() { return frequency; }

    void setShapeIndex(float shapeIndex) { this->shapeIndex = shapeIndex; }

    float getNextValue();
    //==============================================================================

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {}


private:
    // vector of lfo
    std::vector<std::unique_ptr<dsp::Oscillator<float>>> lfoPool;
    float frequency = 2.0; // Hz
    float shapeIndex = 0; // [0, 1]
    double sampleRate = 0;
    int tableResolution = 0;

};
