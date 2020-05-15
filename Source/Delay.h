

#pragma once

#include <vector>
#include <JuceHeader.h>
#include "ProcessorTemplate.h"

/**
 * Feedback delay implementation.
 */
class Delay : public ProcessorTemplate {
public:
    Delay();
    virtual ~Delay();

    /**
     * Sets the delay time in seconds.
     * @param delayTime
     */
    void setDelay(float delayTime) noexcept { this->delayTime = delayTime; } // TODO: check MAX_DELAY

    /**
     * @return delay time in seconds.
     */
    float getDelayTime() { return delayTime; }

    /**
     * Sets the mix between the dry and the wet signal.
     * @param wet
     */
    void setWet(float wet) { this->wet = wet; }

    /**
     * Sets the delay feedback.
     * @param feedback
     */
    void setFeedback(float feedback) { this->feedback = feedback; }

    /**
     * Sets the current sample rate.
     * @param sampleRate
     */
    void setSampleRate(float sampleRate) { this->sampleRate = sampleRate; }

    /**
     * Write a new sample in the circular buffer.
     * @param sample
     */
    void writeNewSample(float sample) noexcept;

    /**
     * Read a sample based on the delay time.
     * Implements a linear interpolation.
     * @return delayed sample
     */
    float readSample() noexcept;

    static const float MAX_DELAY;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept override;
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages, int numberOfSamples) noexcept;

private:
    /**
     * @return size of the buffer.
     */
    size_t size();

    /**
     * Sets the buffer size.
     * @param size
     */
    void setSize(size_t size);

    /**
     * Clear the buffer.
     */
    void clear();

    // circular buffer
    std::vector<float> delayBuffer;

    // index for the circular buffer
    int lastIndex = 0;

    // delay time in seconds
    float delayTime = 0.4;

    // used for mixing the signal
    float wet = 1;

    // feedback of the delay
    float feedback = 0.6;

    // current sample rate
    float sampleRate = 0;
};
