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
        delayLines.push_back(std::make_unique<Delay>());
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 0.1f;
        delayLines[i]->setDelay(delayLines[i]->getDelayTime() + rand);
        bufferPool.push_back(std::unique_ptr<AudioSampleBuffer>(new AudioSampleBuffer(1, 512)));
        lfoPool.push_back(std::unique_ptr<MorphingLfo>(new MorphingLfo(1, 1024)));
    }
}

Chorus::~Chorus() {}

void Chorus::prepareToPlay(double sampleRate, int samplesPerBlock) {
    for (auto &delay: delayLines) {
        delay->prepareToPlay(sampleRate, samplesPerBlock);
    }
    for (auto &buff : bufferPool) {
        // sizing all the parallel buffers
        buff->setSize(1, samplesPerBlock);
    }

    for (auto &lfo : lfoPool) {
        lfo->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void Chorus::releaseResources() {
    for (auto &delay: delayLines) {
        delay->releaseResources();
    }

    for (auto &buff : bufferPool) {
        // clearing all the parallel buffers
        buff->clear(0, buff->getNumSamples());
    }

    for (auto &lfo : lfoPool) {
        lfo->releaseResources();
    }
}

void Chorus::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) {
    bool monoToStereo = false;
    int delayForChannel = 2;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        // iterating per channel
        int numberOfBufferToCopy = (monoToStereo) ? 2 * delayForChannel : delayForChannel;
        for (int i = 0; i < numberOfBufferToCopy; i++) {
            // if mono to stereo, 4 copies of the same channel are performed
            int index = i + channel * delayForChannel;
            bufferPool[index]->copyFrom(0, 0, buffer, channel, 0, buffer.getNumSamples());
        }

        for (int i = 0; i < delayForChannel; i++) {
            // parallel processing with the delay lines
            int index = i + channel * delayForChannel;
            delayLines[index]->processBlock(*(bufferPool[index]), midiMessages);
        }

        // NEW FAST SUM



        // OLD SLOW SUM
        for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); sampleIndex++) {
            // summing delayed and dry signal
            float inputSample = buffer.getSample(channel, sampleIndex);
            float newSample = inputSample * (1 - wet);
            for (int i = 0; i < delayForChannel; i++) {
                int index = i + channel * delayForChannel;
                float wetSample = bufferPool[index]->getSample(0, sampleIndex);
                newSample += (wetSample / delayForChannel) * wet;
            }
            buffer.setSample(channel, sampleIndex, newSample);
        }
    }

}
