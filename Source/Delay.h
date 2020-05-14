

#pragma once

#include <vector>
#include <JuceHeader.h>
#include "ProcessorTemplate.h"

class Delay : public ProcessorTemplate {
public:
    Delay();
    virtual ~Delay();

    size_t size();

    void setDelay(float delayTime) { this->delayTime = delayTime; } // TODO: check MAX_DELAY
    float getDelayTime() { return delayTime; }

    void setWet(float wet) { this->wet = wet; }

    void setFeedback(float feedback) { this->feedback = feedback; }

    void setSampleRate(float sampleRate) { this->sampleRate = sampleRate; }

    void writeNewSample(float sample);
    float readSample();

    static const float MAX_DELAY;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

private:

    void setSize(size_t size);
    void clear();

    std::vector<float> delayBuffer;
    int lastIndex = 0;
    float delayTime = 0.22;
    float wet = 1;
    float feedback = 0.3;
    float sampleRate = 0;
};
