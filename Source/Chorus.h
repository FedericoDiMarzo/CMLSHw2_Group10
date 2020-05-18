

#pragma once

#include <JuceHeader.h>
#include "Delay.h"
#include "MorphingLfo.h"
#include "ProcessorTemplate.h"
//#include "Utils.h"

class Chorus : public ProcessorTemplate {
public:
    Chorus();
    virtual ~Chorus();

    /**
     * Sets the mix between dry and effected signal.
     *
     * @param wet
     */
    void setWet(float wet);

    /**
     * Sets the mix between the chorus and the blur delays.
     *
     * @param blurLevel
     */
    void setBlurLevel(float blurLevel);

    /**
     * Sets the frequency of the delays' lfos.
     *
     * @param frequency
     */
    void setLfoRate(float frequency);

    /**
     * Sets the depth of the delays' lfos.
     *
     * @param intensity
     */
    void setLfoDepth(float intensity);


    /**
     * Sets the feedback for the blur delays.
     *
     * @param blurFeedback
     */
    void setBlurFeedback(float blurFeedback);

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
    std::vector<std::unique_ptr<Delay>> blurDelays;

    // delay time for the blur delays
    float blurDelaysDelayTime = 0.2;

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


    // 0-1 value retrieved from parameters
    float param = 0;
};