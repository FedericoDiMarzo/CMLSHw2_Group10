/*
  ==============================================================================

    Chorus.cpp
    Created: 12 May 2020 11:59:22pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Chorus.h"

#include <memory>


Chorus::Chorus() {
    for (int i = 0; i < 4; i++) {
        // pushing all the delay lines
        delayLines.push_back(std::make_unique<Delay>());
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 0.1f;
        //delayLines[i]->setDelay(delayLines[i]->getDelayTime() + rand);
        delayLines[i]->setWet(1);
        delayLines[i]->setFeedback(feedback);

        // pushing the lfo
        lfoPool.push_back(std::make_unique<MorphingLfo>(1, 1024));
        lfoPool[i]->setFrequency(lfoPool[i]->getFrequency() + rand * 0.8);

        // pushing the buffers
        bufferPool.push_back(std::make_unique<AudioBuffer<float>>());
    }
    setDelay(0.2);
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


void Chorus::applyLfo() noexcept {
    for (int lfoIndex = 0; lfoIndex < 2 * delaysForChannel; lfoIndex++) {
        float lfoValue = lfoPool[lfoIndex]->getNextValue();
        float delayDelta = jmap(lfoValue, -1.0f, 1.0f, -0.1f, 0.1f);
        delayLines[lfoIndex]->setDelay(delayLines[lfoIndex]->getDelayTime() + delayDelta);
    }
}

//==============================================================================

void Chorus::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;
    lfoCounter = 0;

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

    for (auto &lfo : lfoPool) {
        lfo->prepareToPlay(sampleRate / lfoSubRate, samplesPerBlock);
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

    for (auto &lfo : lfoPool) {
        lfo->releaseResources();
    }
}

void Chorus::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {
    int numberOfSamples = buffer.getNumSamples();
    /*for (int sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        int increment = jmin(numberOfSamples - sampleIndex, lfoSubRate - lfoCounter);
        bufferPool[2]->copyFrom(0, 0, buffer, 0, sampleIndex, increment);
        bufferPool[2]->copyFrom(1, 0, buffer, 1, sampleIndex, increment);
        _processBlock(*bufferPool[2], midiMessages, increment);
        buffer.copyFrom(0, sampleIndex, *bufferPool[2], 0, 0, increment);
        buffer.copyFrom(1, sampleIndex, *bufferPool[2], 1, 0, increment);
        sampleIndex += increment;
        lfoCounter += increment;

        // control rate processing
        if (lfoCounter == lfoSubRate) {
            lfoCounter = 0;
            //applyLfo();
        }
    }*/

    _processBlock(buffer, midiMessages, numberOfSamples);

}

void Chorus::_processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages, int numberOfSamples) noexcept {

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

            // adding bufferPool[0] to bufferPool[1]
            float delayGain = 1.0f / static_cast<float>(delaysForChannel);
            bufferPool[1]->addFrom(0, 0, *bufferPool[0],
                    0, 0, numberOfSamples, delayGain);
        }

        // scaling the input based on wet
        buffer.applyGain(channel, 0, numberOfSamples, (1-wet));

        // adding the wet signal
        buffer.addFrom(channel, 0, *bufferPool[1],
                0, 0, numberOfSamples, wet);
    }

}



