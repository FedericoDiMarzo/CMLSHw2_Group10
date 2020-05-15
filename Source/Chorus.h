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
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept override;


private:
    /**
     * Sets the delayTime of a particular delay.
     * @param delayIndex
     * @param delayTime
     */
    void setDelay(int delayIndex, float delayTime) noexcept;

    /**
     * Sets the delayTime for all the delays.
     * @param delayTime
     */
    void setDelay(float delayTime) noexcept;

    /**
     * Apply the lfo to the delay lines.
     */
    void applyLfo() noexcept ;

    /**
     * Implemented for processing chunks of the buffer.
     * @param buffer
     * @param midiMessages
     */
    void _processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages, int numberOfSamples) noexcept;

    //==============================================================================

    // delay lines used to implement the chorus
    std::vector<std::unique_ptr<Delay>> delayLines;

    // number of delay lines for channel
    int delaysForChannel = 2;

    // temporary buffers used during the processing
    std::vector<std::unique_ptr<AudioBuffer<float>>> bufferPool;

    // lfo used for modulating the delay time of the delay lines
    std::vector<std::unique_ptr<MorphingLfo>> lfoPool;

    // control rate
    int lfoSubRate = 10000;

    // counter used to trigger the control update
    int lfoCounter = 0;

    // mix between dry and effected signal
    float wet = 1;

    // feedback of the delays
    float feedback = 0;

    // current sample rate
    float sampleRate = 0;
};