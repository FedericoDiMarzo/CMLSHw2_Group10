/*
  ==============================================================================

    Chorus.cpp
    Created: 12 May 2020 11:59:22pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Chorus.h"


Chorus::Chorus() {
    // pushing the short delay lines
    for (int i = 0; i < delaysForChannel * 2; i++) {
        delayLines.push_back(std::make_unique<Delay>());
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 2;
        delayLines[i]->setWet(1);
        delayLines[i]->setFeedback(0);
        delayLines[i]->setLfoSpeed(3.0f + rand * 0.2f);
    }
    delayLines[0]->setDelay(0.0022);
    delayLines[1]->setDelay(0.0055);
    delayLines[2]->setDelay(0.0033);
    delayLines[3]->setDelay(0.0045);

    // pushing the blur delay lines
    for (int i = 0; i < 2; i++) {
        float rand = Random::getSystemRandom().nextFloat();
        rand = jmap(rand, 0.001f, 0.005f);
        blurDelays.push_back(std::make_unique<Delay>());
        blurDelays[i]->setDelay(blurDelaysDelayTime + rand);
        blurDelays[i]->setWet(1);
        blurDelays[i]->setFeedback(blurFeedback);
    }

    // pushing the buffers
    for (int i = 0; i < 3; i++) {
        bufferPool.push_back(std::make_unique<AudioBuffer<float>>());
    }
}

Chorus::~Chorus() {}

void Chorus::setDelay(int delayIndex, float delayTime) noexcept {
    delayLines[delayIndex]->setDelay(delayTime);
}

void Chorus::setDelay(float delayTime) noexcept {
    for (int i = 0; i < delayLines.size(); i++) {
        setDelay(i, delayTime);
    }
}

void Chorus::setWet(float wet) {
    jassert(wet >= 0);
    jassert(wet <= 1);
    this->wet = wet;
}

void Chorus::setBlurLevel(float blurLevel) {
    jassert(blurLevel >= 0);
    jassert(blurLevel <= 1);
    this->blurLevel = blurLevel;
}

void Chorus::setBlurFeedback(float blurFeedback) {
    jassert(blurFeedback >= 0);
    jassert(blurFeedback < 1);
    this->blurFeedback = blurFeedback;
    for (auto &delay : blurDelays) {
        delay->setFeedback(blurFeedback);
    }
}

void Chorus::setLfoRate(float param) {
    float frequency = jmap(param, 0.5f, 6.0f);
    float randScale = 0.3;
    jassert(frequency - randScale > 0);
    for (int i = 0; i < delaysForChannel * 2; i++) {
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 2;
        delayLines[i]->setLfoSpeed(frequency + rand * randScale);
    }
}

void Chorus::setStereoEnhance(float param) {
    float stereoEnhance = jmap(param, 0.0f, 2.0f);
    jassert(stereoEnhance >= 0);
    this->stereoEnhance = stereoEnhance;
}

void Chorus::setIntensity(float param) {
    jassert(param >= 0);
    this->intensity = param;
}

void Chorus::setLfoDepth(float param) {
    
    float intensity = jmap(param, 0.0001f, 0.0015f);

    jassert(intensity >= 0);
    jassert(intensity < 0.002);
    for (int i = 0; i < delaysForChannel * 2; i++) {
        delayLines[i]->setLfoIntensity(intensity);
    }
}

//==============================================================================

void Chorus::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;

    for (auto &delay: delayLines) {
        delay->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (int i = 0; i < bufferPool.size(); i++) {
        // sizing all the additional buffers
        if (i < 2) {
            // mono buffers
            bufferPool[i]->setSize(1, samplesPerBlock);
        } else {
            // stereo buffers
            bufferPool[i]->setSize(2, samplesPerBlock);
        }
        bufferPool[i]->clear();
    }
}

void Chorus::releaseResources() {
    for (auto &delay: delayLines) {
        delay->releaseResources();
    }

    for (auto &buff : bufferPool) {
        // clearing all the parallel buffers
        buff->clear();
    }
}


void Chorus::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {

    int numberOfSamples = buffer.getNumSamples();
    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {

        // resetting the wet buffer
        bufferPool[1]->clear(0, 0, numberOfSamples);

        // iterating for the number of delays of each channel
        for (int i = 0; i < delaysForChannel; i++) {
            // copy of the input
            bufferPool[0]->copyFrom(0, 0, buffer,
                                    channel, 0, numberOfSamples);

            // actual index for delayLines
            int delayIndex = i + channel * delaysForChannel;

            // processing bufferPool[0]
            delayLines[delayIndex]->processBlock(*bufferPool[0], midiMessages);

            // the intensity parameter influences the second delay
            float delayGain = 1.0f / static_cast<float>(delaysForChannel);
            delayGain = (i == 2) ? delayGain * intensity : delayGain;

            // adding bufferPool[0] to bufferPool[1]
            bufferPool[1]->addFrom(0, 0, *bufferPool[0],
                                   0, 0, numberOfSamples, delayGain);
        }

        // blur delays
        bufferPool[0]->copyFrom(0, 0, buffer,
                                channel, 0, numberOfSamples);
        blurDelays[channel]->processBlock(*bufferPool[0], midiMessages);
        //buffer.applyGain(channel, 0, numberOfSamples, 1 - blurMix);
        bufferPool[1]->addFrom(0, 0, *bufferPool[0],
                               0, 0, numberOfSamples, blurLevel);

        // scaling the input based on wet
        buffer.applyGain(channel, 0, numberOfSamples, (1 - wet));

        // adding the wet signal
        buffer.addFrom(channel, 0, *bufferPool[1],
                       0, 0, numberOfSamples, wet);

        // todo test
        // blurDelays[channel]->processBlock(buffer, midiMessages);
    }

    // MID SIDE processing
    if (buffer.getNumChannels() == 2) {
        // midside processing applied on a copy in order to do dry/mix
        bufferPool[2]->copyFrom(0, 0, buffer,
                                0, 0, numberOfSamples);
        bufferPool[2]->copyFrom(1, 0, buffer,
                                1, 0, numberOfSamples);
        utils::midSideEncoding(*bufferPool[2]);
        buffer.applyGain(0, 0, numberOfSamples, stereoEnhance);
        utils::midSideDecoding(*bufferPool[2]);

        // dry wet mix
        buffer.applyGain(1 - wet);
        buffer.addFrom(0, 0, *bufferPool[2],
                       0, 0, numberOfSamples, wet);
        buffer.addFrom(1, 0, *bufferPool[2],
                       1, 0, numberOfSamples, wet);
    }

}














