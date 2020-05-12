/*
  ==============================================================================

    Delay.h
    Created: 12 May 2020 5:52:45pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#pragma once
#include <vector>
#include <JuceHeader.h>

class Delay : public AudioProcessor{
public:
    Delay();
    virtual ~Delay();

    size_t size();
    void setSize(size_t size);
    void clear();
    void setDelay(float delayTime) {this->delayTime = delayTime;}
    void setWet(float wet) { this->wet = wet;}
    void setFeedback(float feedback) { this->feedback = feedback;}
    void setSampleRate(float sampleRate) { this->sampleRate = sampleRate;}
    void writeNewSample(float sample);
    float readSample();

    static const float MAX_DELAY;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float> &buffer, MidiBuffer &midiMessages) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override          { return nullptr; }
    bool hasEditor() const override                        { return false; }

    //==============================================================================
    const String getName() const override                  { return {}; }
    bool acceptsMidi() const override                      { return false; }
    bool producesMidi() const override                     { return false; }
    double getTailLengthSeconds() const override           { return 0; }

    //==============================================================================
    int getNumPrograms() override                          { return 0; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const String getProgramName (int) override             { return "Delay"; }
    void changeProgramName (int, const String&) override   {}

    //==============================================================================
    void getStateInformation (MemoryBlock&) override       {}
    void setStateInformation (const void*, int) override   {}

private:
    std::vector<float> delayBuffer;
    int lastIndex = 0;
    float delayTime = 0.2;
    float wet = 0.8;
    float feedback = 0.15;
    float sampleRate = 0;
};
