/*
  ==============================================================================

    Chorus.cpp
    Created: 12 May 2020 11:59:22pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Chorus.h"


Chorus::Chorus()
        : AudioProcessor(
        BusesProperties().withInput("Input", AudioChannelSet::stereo(), true)
                .withOutput("Output", AudioChannelSet::stereo(), true)) {
    delayLines.push_back(new Delay());
    parallelBuffers.push_back(new AudioBuffer<float>());
}

Chorus::~Chorus() {}

void Chorus::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;
    for (int i = 0; i < delayLines.size(); i++) {
        delayLines[i]->prepareToPlay(sampleRate, samplesPerBlock);
        parallelBuffers[i]->setSize(1, samplesPerBlock);
    }
}

void Chorus::releaseResources() {
    for (auto & delayLine : delayLines) {
        delayLine->clear();
    }
}

void Chorus::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) {
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = 0; i < delayLines.size(); i++) {
        parallelBuffers[i]->copyFrom(0, 0, buffer.getReadPointer(0), buffer.getNumSamples());
        delayLines[i]->processBlock(*parallelBuffers[i], midiMessages);
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        // iterating per channel
        if (channel < totalNumOutputChannels) {
            // checking for bus coherence
            float *audioBufferData = buffer.getWritePointer(channel);
            for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); sampleIndex++) {
                // processing
                float audioBufferSample = audioBufferData[sampleIndex];
                audioBufferSample *= (1-wet);
                for (auto &b : parallelBuffers) {
                    float *parallelBufferData = b->getWritePointer(channel);
                }

                audioBufferData[sampleIndex] = audioBufferSample;
            }
        }
    }
}
