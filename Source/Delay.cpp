/*
  ==============================================================================

    Delay.cpp
    Created: 12 May 2020 5:52:45pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Delay.h"
#include "Utils.h"

const float Delay::MAX_DELAY = 10.0; // in seconds

Delay::Delay() {}

Delay::~Delay() {}

size_t Delay::size() {
    return delayBuffer.size();
}

void Delay::setSize(size_t size) {
    delayBuffer.resize(size);
}

void Delay::clear() {
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0);
    lastIndex = 0;
}


void Delay::writeNewSample(float sample) noexcept {
    if (size() == 0) {
        // avoid zero divisions
        return;
    }

    lastIndex %= size(); // circular wrapping
    delayBuffer[lastIndex++] = sample;
}

float Delay::readSample() noexcept {
    if (size() == 0) {
        // avoids zero divisions
        return 0;
    }

    float readIndex = static_cast<float>(lastIndex) - (delayTime * sampleRate);
    if (readIndex < 0) {
        // underflow wrapping
        readIndex = static_cast<float>(size()) - readIndex;
    }
    float fract = readIndex - (long) readIndex; // decimal residual
    int readIndexInt = std::floor(readIndex); // integer residual
    readIndexInt %= size(); // overflow wrapping
    int nextRead = (readIndexInt + 1) % size(); // second value used for interpolation
    float interpolatedValue = utils::interpolate(delayBuffer[readIndexInt], delayBuffer[nextRead], fract);
    return interpolatedValue;

}

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock) {
    clear();
    setSampleRate(sampleRate);
    setSize(juce::roundToInt(MAX_DELAY * sampleRate));
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
            float delayedSample = readSample();
            float newSample = wet * delayedSample + (1 - wet) * inputBufferSample;
            writeNewSample((1 - feedback) * inputBufferSample + feedback * delayedSample);
            buffer.setSample(channel, sampleIndex, newSample);
        }
    }
}

void Delay::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {
    processBlock(buffer, midiMessages, buffer.getNumSamples());
}


