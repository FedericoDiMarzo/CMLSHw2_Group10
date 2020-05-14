
#pragma once
#include <JuceHeader.h>

/**
 * Template class used for defining an audio processor.
 */
class ProcessorTemplate {
public:
    ProcessorTemplate() = default;
    virtual ~ProcessorTemplate() = default;

    //==============================================================================
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock) = 0;
    virtual void releaseResources() = 0;
    virtual void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) = 0;
};