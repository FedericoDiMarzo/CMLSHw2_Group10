
#include "Chorus.h"


Chorus::Chorus() {
    // pushing the short delay lines
    for (int i = 0; i < delaysForChannel * 2; i++) {
        delayLines.push_back(std::make_unique<Delay>());
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 2;
        delayLines[i]->setWet(1);
        delayLines[i]->setFeedback(0);
        delayLines[i]->setLfoSpeed(3.0f + rand * 0.2f);
    }
    delayLines[0]->setDelay(0.0022);
    delayLines[1]->setDelay(0.0055);
    delayLines[2]->setDelay(0.0033);
    delayLines[3]->setDelay(0.0045);

    // pushing the blur delay lines

    float rand = Random::getSystemRandom().nextFloat();
    rand = jmap(rand, 0.001f, 0.005f);
    blurStereoDelay.setDelay(blurDelayTime + rand);
    blurStereoDelay.setWet(1);
    blurStereoDelay.setFeedback(blurFeedback);


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

void Chorus::setWet(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    this->wet = param;
}

void Chorus::setBlurLevel(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    param = jmap(param, 0.0f, 0.4f);
    this->blurLevel = param;
}

void Chorus::setBlurFeedback(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    param = jmap(param, 0.0f, 0.5f);
    this->blurFeedback = param;
    blurStereoDelay.setFeedback(blurFeedback);
}

void Chorus::setLfoRate(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    float frequency = jmap(param, 0.5f, 8.0f);
    float randScale = 0.3;
    for (int i = 0; i < delaysForChannel * 2; i++) {
        float rand = (Random::getSystemRandom().nextFloat() - 0.5f) * 2;
        delayLines[i]->setLfoSpeed(frequency + rand * randScale);
    }
}

void Chorus::setStereoEnhance(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    float stereoEnhance = jmap(param, 0.0f, 2.0f);
    this->stereoEnhance = stereoEnhance;
}

void Chorus::setIntensity(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    this->intensity = param;
}

void Chorus::setLfoDepth(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    float intensity = jmap(param, 0.0001f, 0.0015f);

    for (int i = 0; i < delaysForChannel * 2; i++) {
        delayLines[i]->setLfoIntensity(intensity);
    }
}

void Chorus::setBlurDelayTime(float param) {
    jassert(param >= 0);
    jassert(param <= 1);
    float blurDelayTime = jmap(param, 0.0f, 1.0f);
    this->blurDelayTime = blurDelayTime;
    blurStereoDelay.setDelay(blurDelayTime);
}

//==============================================================================

void Chorus::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;

    // initializing main delay lines
    for (auto &delay: delayLines) {
        delay->prepareToPlay(sampleRate, samplesPerBlock);
    }

    // initializing blur delay
    blurStereoDelay.prepareToPlay(sampleRate, samplesPerBlock);

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

    blurStereoDelay.releaseResources();

    for (auto &buff : bufferPool) {
        // clearing all the parallel buffers
        buff->clear();
    }
}


void Chorus::processBlock(AudioBuffer<float> &buffer, MidiBuffer &midiMessages) noexcept {

    int numberOfSamples = buffer.getNumSamples();
    for (int channel = 0; channel < buffer.getNumChannels(); channel++) {

        // resetting the wet buffer
        bufferPool[1]->clear(0, 0, numberOfSamples);

        // iterating for the number of delays of each channel
        for (int i = 0; i < delaysForChannel; i++) {
            // copy of the input
            bufferPool[0]->copyFrom(0, 0, buffer,
                                    channel, 0, numberOfSamples);

            // actual index for delayLines
            int delayIndex = i + channel * delaysForChannel;

            // processing bufferPool[0]
            delayLines[delayIndex]->processBlock(*bufferPool[0], midiMessages);

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

    // blur delay
    bufferPool[2]->copyFrom(0, 0, buffer,
                            0, 0, numberOfSamples);
    bufferPool[2]->copyFrom(1, 0, buffer,
                            1, 0, numberOfSamples);
    blurStereoDelay.processBlock(*bufferPool[2], midiMessages);
    //buffer.applyGain(channel, 0, numberOfSamples, 1 - blurMix);
    buffer.addFrom(0, 0, *bufferPool[2],
                           0, 0, numberOfSamples, blurLevel);
    buffer.addFrom(1, 0, *bufferPool[2],
                   1, 0, numberOfSamples, blurLevel);

    // MID SIDE processing
    if (buffer.getNumChannels() == 2) {
        // midside processing applied on a copy in order to do dry/mix
        bufferPool[2]->copyFrom(0, 0, buffer,
                                0, 0, numberOfSamples);
        bufferPool[2]->copyFrom(1, 0, buffer,
                                1, 0, numberOfSamples);

        utils::enhanceStereo(*bufferPool[2], stereoEnhance);

        // dry wet mix
        buffer.applyGain(1 - wet);
        buffer.addFrom(0, 0, *bufferPool[2],
                       0, 0, numberOfSamples, wet);
        buffer.addFrom(1, 0, *bufferPool[2],
                       1, 0, numberOfSamples, wet);
    }

    // gain compensation for the mix knob:
    // it's always better to check the effected and dry signal with
    // the same levels!
    float compensationAmount = 0.4; 
    buffer.applyGain(1 + wet * compensationAmount);

}
















