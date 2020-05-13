/*
  ==============================================================================

    Delay.cpp
    Created: 12 May 2020 5:52:45pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "Delay.h"
#include "Utils.h"

const float Delay::MAX_DELAY = 10.0;

Delay::Delay()
        : AudioProcessor(
        BusesProperties().withInput("Input", AudioChannelSet::mono(), true)
                .withOutput("Output", AudioChannelSet::mono(), true)) {}

Delay::~Delay() {}

size_t Delay::size() {
    /**
     * Size of the buffer.
     */
    return delayBuffer.size();
}

void Delay::setSize(size_t size) {
    /**
     * Sets the size of the buffer.
     */
    delayBuffer.resize(size);
}

void Delay::clear() {
    /**
     * Clears the buffer.
     */
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0);
}


void Delay::writeNewSample(float sample) {
    /**
     * Writes a new sample in the buffer, and
     * increments index.
     */
    if (size() == 0) {
        // avoid zero divisions
        return;
    }

    lastIndex %= size(); // circular wrapping
    delayBuffer[lastIndex++] = sample;
}

float Delay::readSample() {
    /**
     * Reads a sample, depending on the delay time.
     * Linear interpolation is performed in the buffer.
     */

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
    int readIndexInt = juce::roundToInt(readIndex); // integer residual
    readIndexInt %= size(); // overflow wrapping
    int nextRead = (readIndexInt + 1) % size(); // second value used for interpolation
    return Utils::interpolate(delayBuffer[readIndexInt], delayBuffer[nextRead], fract);

}

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock) {
    clear();
    setSampleRate(sampleRate);
    setDelay(delayTime * Random::getSystemRandom().nextFloat() * 2); // random initialization
    setSize(juce::roundToInt(MAX_DELAY * sampleRate));
}

void Delay::releaseResources() {
    clear();
}

void Delay::processBlock(AudioBuffer<float> &audioBuffer, MidiBuffer &midiMessages) {
    // bus info
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        // iterating per channel
        if (channel < totalNumOutputChannels) {
            // checking for bus coherence
            float *audioBufferData = audioBuffer.getWritePointer(channel);
            for (int sampleIndex = 0; sampleIndex < audioBuffer.getNumSamples(); sampleIndex++) {
                // processing
                float audioBufferSample = audioBufferData[sampleIndex];
                float delayedSample = readSample();
                float newSample = wet * delayedSample + (1 - wet) * audioBufferSample;
                writeNewSample(newSample);
                audioBufferData[sampleIndex] = newSample;
            }
        }
    }
}


