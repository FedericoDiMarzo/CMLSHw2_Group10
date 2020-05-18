

#pragma once

#include <JuceHeader.h>
#include "Delay.h"
#include "CustomLfo.h"
#include "ProcessorTemplate.h"

class Chorus : public ProcessorTemplate {
public:
    Chorus();
    virtual ~Chorus();

    /**
     * Sets the mix between dry and effected signal.
     *
     * @param param
     */
    void setWet(float param);

    /**
     * Sets the mix between the chorus and the blur delays.
     *
     * @param param
     */
    void setBlurLevel(float param);

    /**
     * Sets the frequency of the delays' lfos.
     *
     * @param frequency
     */
    void setLfoRate(float frequency);

    /**
     * Sets the blur of the delays' lfos.
     *
     * @param intensity
     */
    void setLfoDepth(float intensity);


    /**
     * Sets the feedback for the blur delays.
     *
     * @param param
     */
    void setBlurFeedback(float param);

    /**
     * Sets the gain of the side channel.
     *
     * @param stereoEnhance
     */
    void setStereoEnhance(float stereoEnhance);

    /**
     * Sets the gain of one of the delayLine.
     *
     * @param intensity
     */
    void setIntensity(float intensity);

    /**
     * Set the delay time for the blur delay.
     *
     * @param blurDelayTime
     */
    void setBlurDelayTime(float blurDelayTime);

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


    //==============================================================================

    // delay lines used to implement the chorus
    std::vector<std::unique_ptr<Delay>> delayLines;

    // additional delay lines used for blur knob
    Delay blurStereoDelay;

    // delay time for the blur delays
    float blurDelayTime = 0.08;

    // blur amount
    float blurLevel = 0;

    // number of delay lines for channel
    int delaysForChannel = 2;

    // temporary buffers used during the processing
    std::vector<std::unique_ptr<AudioBuffer<float>>> bufferPool;

    // mix between dry and effected signal
    float wet = 0.4;

    // feedback of the blur delays
    float blurFeedback = 0;

    // current sample rate
    float sampleRate = 0;

    // side channel gain
    float stereoEnhance = 1;

    // gain of one of the delayLine
    float intensity = 1;

};