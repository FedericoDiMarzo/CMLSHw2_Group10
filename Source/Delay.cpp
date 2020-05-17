
#include "Delay.h"


const float Delay::MAX_DELAY = 10; // in seconds

Delay::Delay()
        :
        lfo(1024) {
    setLfoSpeed(3.0);
}

Delay::~Delay() {}

int Delay::size() {
    return delayBuffer.getNumSamples();
}

void Delay::setSize(size_t size) {
    delayBuffer.setSize(2, static_cast<int>(size));
}

void Delay::clear() {
    delayBuffer.clear();
    lastIndex[0] = 0;
    lastIndex[1] = 0;
}

void Delay::setFeedback(float feedback) {
    jassert(feedback >= 0);
    jassert(feedback < 1);
    this->feedback = feedback;
}

void Delay::setLfoSpeed(float frequency) {
    jassert(frequency > 0);
    lfo.setFrequency(frequency);
}

void Delay::setLfoIntensity(float intensity) {
    jassert(intensity <= 0.0000003);
    lfoIntensity = intensity;
}

void Delay::applyLfo() noexcept {
    float lfoValue = lfo.getNextValue();
    float delayDelta = jmap(lfoValue, -1.0f, 1.0f,
                            -lfoIntensity, lfoIntensity);

    jassert(delayTime + delayDelta > 0);
    jassert(delayTime + delayDelta < MAX_DELAY);

    setDelay(delayTime + delayDelta);
}

void Delay::writeNewSample(int channel, float sample) noexcept {
    jassert(size() > 0);
    jassert(channel >= 0);
    jassert(channel < 2);

    // circular wrapping
    delayBuffer.setSample(channel, lastIndex[channel], sample);
    lastIndex[channel]++;
    lastIndex[channel] %= size();
}

float Delay::readSample(int channel) noexcept {
    jassert(size() > 0);
    jassert(channel >= 0);
    jassert(channel < 2);

    float readIndex = static_cast<float>(lastIndex[channel]) - (delayTime * sampleRate);
    if (readIndex < 0) {
        // underflow wrapping
        readIndex = static_cast<float>(size()) + readIndex;
    }

    float fract = readIndex - (long) readIndex; // decimal residual
    int readIndexInt = std::floor(readIndex); // integer residual
    readIndexInt %= size(); // overflow wrapping
    int nextRead = (readIndexInt + 1) % size(); // second value used for interpolation
    float interpolatedValue = utils::interpolate(delayBuffer.getSample(channel, readIndexInt),
                                                 delayBuffer.getSample(channel, nextRead), fract);

    return interpolatedValue;

}


//==============================================================================

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock) {
    clear();
    setSampleRate(sampleRate);
    setSize(std::ceil(MAX_DELAY * sampleRate));
    lfo.prepareToPlay(sampleRate, samplesPerBlock);
}

void Delay::releaseResources() {
    clear();
    lfo.releaseResources();
}

void Delay::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages, int numberOfSamples) noexcept {

    // iterating per channel
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        for (int sampleIndex = 0; sampleIndex < numberOfSamples; sampleIndex++) {
            // processing
            float inputBufferSample = buffer.getSample(channel, sampleIndex);
            float delayedSample = readSample(channel);
            float newSample = wet * delayedSample + (1 - wet) * inputBufferSample;
            writeNewSample(channel, inputBufferSample + feedback * delayedSample);
            buffer.setSample(channel, sampleIndex, newSample);
            applyLfo();
        }
    }
}

void Delay::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {
    processBlock(buffer, midiMessages, buffer.getNumSamples());
}




