/*
  ==============================================================================

    Delay.cpp
    Created: 12 May 2020 5:52:45pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Delay.h"
#include "Utils.h"

const float Delay::MAX_DELAY = 2; // in seconds

Delay::Delay() {}

Delay::~Delay() {}

int Delay::size() {
    return delayBuffer.getNumSamples();
}

void Delay::setSize(size_t size) {
    delayBuffer.setSize(2, size);
}

void Delay::clear() {
    delayBuffer.clear();
    lastIndex = 0;
}


void Delay::writeNewSample(int channel, float sample) noexcept {
    // avoid zero divisions
    if (size() == 0) {
        return;
    }

    // circular wrapping
    delayBuffer.setSample(channel, lastIndex, sample);
    lastIndex++;
    lastIndex %= size();
}

float Delay::readSample(int channel) noexcept {
    // avoids zero divisions
    if (size() == 0) {
        return 0;
    }

    float readIndex = static_cast<float>(lastIndex) - (delayTime * sampleRate);
    if (readIndex < 0) {
        // underflow wrapping
        readIndex = static_cast<float>(size()) - readIndex;
    }
    float fract = readIndex - (long) readIndex; // decimal residual
    int readIndexInt = std::floor(readIndex); // integer residual
    if (lastIndex == 0) {
        int a = 0;
    }
    readIndexInt %= size(); // overflow wrapping
    int nextRead = (readIndexInt + 1) % size(); // second value used for interpolation
    float interpolatedValue = utils::interpolate(delayBuffer.getSample(channel, readIndexInt),
            delayBuffer.getSample(channel, nextRead), fract);
    return interpolatedValue;

}

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock) {
    clear();
    setSampleRate(sampleRate);
    setSize(std::ceil(MAX_DELAY * sampleRate));
}

void Delay::releaseResources() {
    clear();
}

void Delay::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages, int numberOfSamples) noexcept {

    // iterating per channel
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        for (int sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
            // processing
            float inputBufferSample = buffer.getSample(channel, sampleIndex);
            float delayedSample = readSample(channel);
            float newSample = wet * delayedSample + (1 - wet) * inputBufferSample;
            writeNewSample(channel, (1 - feedback) * inputBufferSample + feedback * delayedSample);
            buffer.setSample(channel, sampleIndex, newSample);
        }
    }
}

void Delay::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {
    processBlock(buffer, midiMessages, buffer.getNumSamples());
}


