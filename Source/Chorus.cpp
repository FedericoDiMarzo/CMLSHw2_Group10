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
        : AudioProcessor(
        BusesProperties().withInput("Input", AudioChannelSet::stereo(), true)
                .withOutput("Output", AudioChannelSet::stereo(), true)) {
    for (int i = 0; i < 4; i++) {
        delayLines.push_back(std::make_unique<Delay>());
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 0.1f;
        delayLines[i]->setDelay(delayLines[i]->getDelayTime() + rand);
        bufferPool.push_back(std::unique_ptr<AudioSampleBuffer>(new AudioSampleBuffer(1, 512)));
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
}

void Chorus::releaseResources() {
    for (auto &delay: delayLines) {
        delay->releaseResources();
    }

    for (auto &buff : bufferPool) {
        // clearing all the parallel buffers
        buff->clear(0, buff->getNumSamples());
    }
}

void Chorus::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) {
    bool monoToStereo = getTotalNumInputChannels() < getTotalNumOutputChannels();
    int delayForChannel = 2;
    for (int channel = 0; channel < getTotalNumInputChannels(); ++channel) {
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

        float *inputBufferData = buffer.getWritePointer(channel);
        

        for (int sampleIndex = 0; sampleIndex < buffer.getNumSamples(); sampleIndex++) {
            // summing delayed and dry signal
            float inputBufferSample = inputBufferData[sampleIndex];
            float newSample = inputBufferSample * (1 - wet);
            for (int i = 0; i < delayForChannel; i++) {
                int index = i + channel * delayForChannel;
                const float *delayBufferData = bufferPool[index]->getReadPointer(0);
                float tmp = delayBufferData[sampleIndex];
                newSample += (delayBufferData[sampleIndex] / delayForChannel) * wet;
            }
            inputBufferData[sampleIndex] = newSample;
        }
    }

}
