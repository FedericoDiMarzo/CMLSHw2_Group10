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
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f);
        delayLines[i]->setDelay(delayLines[i]->getDelayTime());
        delayLines[i]->setWet(1);
        delayLines[i]->setFeedback(0);
        delayLines[i]->setLfoSpeed(3.0f + rand * 0.2f);
    }

    // pushing the blur delay lines
    for (int i = 0; i < 2; i++) {
        blurDelays.push_back(std::make_unique<Delay>());
        delayLines[i]->setDelay(blurDelaysDelayTime);
        delayLines[i]->setWet(blurMix);
        delayLines[i]->setFeedback(blurFeedback);
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

void Chorus::setBlurMix(float blurMix) {
    jassert(wet >= 0);
    jassert(wet <= 1);
    this->blurMix = blurMix;
}

void Chorus::setBlurFeedback(float blurFeedback) {
    jassert(wet >= 0);
    jassert(wet < 1);
    this->blurFeedback = blurFeedback;
}

void Chorus::setLfoRate(float frequency) {
    float randScale = 0.3;
    jassert(frequency - randScale > 0);
    for (int i = 0; i < delaysForChannel * 2; i++) {
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f);

        delayLines[i]->setLfoSpeed(3 + rand * randScale);
    }
}

void Chorus::setStereoEnhance(float stereoEnhance) {
    jassert(stereoEnhance >= 0);
    this->stereoEnhance = stereoEnhance;
}

void Chorus::setIntensity(float intensity) {
    jassert(intensity >= 0);
    this->intensity = intensity;
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
        // copy of the input
        bufferPool[0]->copyFrom(0, 0, buffer, channel, 0, numberOfSamples);

        // resetting the wet buffer
        bufferPool[1]->clear(0, 0, numberOfSamples);

        // iterating for the number of delays of each channel
        for (int i = 0; i < delaysForChannel; i++) {
            // actual index for delayLines
            int delayIndex = i + channel * delaysForChannel;

            // processing bufferPool[0]
            delayLines[delayIndex]->processBlock(*bufferPool[0], midiMessages, numberOfSamples);

            // the intensity parameter influences the second delay
            float delayGain = 1.0f / static_cast<float>(delaysForChannel);
            delayGain = (i == 2) ? delayGain * intensity : delayGain;

            // adding bufferPool[0] to bufferPool[1]
            bufferPool[1]->addFrom(0, 0, *bufferPool[0],
                                   0, 0, numberOfSamples, delayGain);
        }


        // scaling the input based on wet
        buffer.applyGain(channel, 0, numberOfSamples, (1 - wet));

        // adding the wet signal
        buffer.addFrom(channel, 0, *bufferPool[1],
                       0, 0, numberOfSamples, wet);
    }

}














