

#pragma once

#include <JuceHeader.h>
#include "ProcessorTemplate.h"
#include "MorphingLfo.h"
#include "Utils.h"

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
    void setDelay(float delayTime) noexcept { this->delayTime = delayTime; }

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
    void setFeedback(float feedback);

    /**
     * Sets the speed of the lfo.
     *
     * @param frequency
     */
    void setLfoSpeed(float frequency);

    /**
     * Sets the intensity of the lfo.
     *
     * @param intensity
     */
    void setLfoIntensity(float intensity);

    /**
     * Sets the current sample rate.
     * @param sampleRate
     */
    void setSampleRate(float sampleRate) { this->sampleRate = sampleRate; }

    /**
     * Write a new sample in the circular buffer.
     * @param sample
     */
    void writeNewSample(int channel, float sample) noexcept;

    /**
     * Read a sample based on the delay time.
     * Implements a linear interpolation.
     * @return delayed sample
     */
    float readSample(int channel) noexcept;

    static const float MAX_DELAY;

    //==============================================================================

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept override;


private:
    /**
     * @return size of the buffer.
     */
    int size();

    /**
     * Sets the buffer size.
     * @param size
     */
    void setSize(size_t size);

    /**
     * Clear the buffer.
     */
    void clear();

    /**
     * Modulates the delayTime, based on the
     * lfoIntensity.
     */
    void applyLfo() noexcept;

    // circular buffer
    AudioBuffer<float> delayBuffer;

    // index for the circular buffer
    int lastIndex[2];

    // delay time in seconds
    float delayTime = 0.03;

    // used for mixing the signal
    float wet = 1;

    // feedback of the delay
    float feedback = 0;

    // current sample rate
    float sampleRate = 0;

    // lfo
    MorphingLfo lfo;

    // lfo intensity over the delayTime
    float lfoIntensity = 0.0000001;

    // modulation applied to the delayTime
    float delayDelta = 0;

};
