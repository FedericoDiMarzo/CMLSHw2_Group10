
#pragma once

#include <JuceHeader.h>
#include "ProcessorTemplate.h"

class MorphingLfo : public ProcessorTemplate {
public:
    MorphingLfo(int tableResolution);
    virtual ~MorphingLfo();

    void setFrequency(float frequency);

    float getFrequency() { return frequency; }

    float getNextValue();
    //==============================================================================

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override {}


private:
    // actual lfo used
    dsp::Oscillator<float> lfo;

    // frequency of the lfo
    float frequency = 2.0; // Hz

    // current sample rate
    double sampleRate = 0;

    // resolution of the lookup table
    int tableResolution = 0;

};
