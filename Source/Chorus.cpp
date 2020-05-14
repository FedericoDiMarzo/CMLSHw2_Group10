/*
  ==============================================================================

    Chorus.cpp
    Created: 12 May 2020 11:59:22pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Chorus.h"

#include <memory>


Chorus::Chorus()
 {
    for (int i = 0; i < 4; i++) {
        // pushing all the delay lines
        delayLines.push_back(std::make_unique<Delay>());
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 0.1f;
        delayLines[i]->setDelay(delayLines[i]->getDelayTime() + rand);
        delayLines[i]->setWet(1);

        // pushing the lfo
        lfoPool.push_back(std::unique_ptr<MorphingLfo>(new MorphingLfo(1, 1024)));
        lfoPool[i]->setFrequency(lfoPool[i]->getFrequency() + rand * 0.8);

        // pushing the buffers
        bufferPool.push_back(std::make_unique<AudioBuffer<float>>());
    }
}

Chorus::~Chorus() {}

void Chorus::setDelay(int delayIndex, float delayTime) noexcept {
    for (int i = 0; i < delaysForChannel; i++) {
        int index = delayIndex * delaysForChannel + i;
        delayLines[index]->setDelay(delayTime);
    }
}

void Chorus::setDelay(float delayTime) noexcept {
    setDelay(0, delayTime);
    setDelay(1, delayTime);
}


void Chorus::applyLfo() noexcept {
    lfoCounter = 0;
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

    tmpAudioBlock = std::make_unique<dsp::AudioBlock<float>>(heapBlock, 2, samplesPerBlock);
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
    /*int numberOfSamples = buffer.getNumSamples();

    auto audioBlock = tmpAudioBlock->getSubBlock(0, buffer.getNumSamples());
    audioBlock.copyFrom(buffer, 0, 0, numberOfSamples);

    for (int sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
        int increment = jmin(numberOfSamples - sampleIndex, lfoSubRate - lfoCounter);
        auto subBlock = audioBlock.getSubBlock(sampleIndex, increment);
        bufferPool[3]->copyFrom(0, 0, subBlock.getChannelPointer(0), static_cast<int>(subBlock.getNumSamples()));
        bufferPool[3]->copyFrom(1, 0, subBlock.getChannelPointer(1), static_cast<int>(subBlock.getNumSamples()));
        _processBlock(*bufferPool[3], midiMessages);
        sampleIndex += increment;
        lfoCounter += increment;

        // control rate processing
        if (lfoCounter == lfoSubRate) {
            applyLfo();
        }
    }*/
    _processBlock(buffer, midiMessages);

}

void Chorus::_processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {
    //bool monoToStereo = false;
    bufferPool[1]->clear(); // clearing the wet accumulation buffer

    // iterating per channel
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        for (int i = 0; i < delaysForChannel; i++) {

            // obtaining the index for delayLines
            int index = i + delaysForChannel * channel;

            // copying the input buffer in bufferPool[0]
            bufferPool[0]->copyFrom(0, 0, buffer,
                                   channel, 0, buffer.getNumSamples());

            // processing
            delayLines[index]->processBlock(*bufferPool[0], midiMessages);

            // accumulating the output of the delay lines
            bufferPool[1]->addFrom(0, 0, *bufferPool[0],
                                  0, 0, buffer.getNumSamples());

        }

        // scaling the wet based on the number of delays
        bufferPool[1]->applyGain(1.0 / delaysForChannel * wet);

        // scaling the dry signal
        //buffer.applyGain(1-wet);

        // summing dry and wet signals
        buffer.addFrom(channel, 0, *bufferPool[1],
                       0, 0, buffer.getNumSamples());

    }
}



