/*
  ==============================================================================

    MorphingLfo.cpp
    Created: 14 May 2020 9:45:39pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "MorphingLfo.h"

MorphingLfo::MorphingLfo(int tableResolution) {
    this->tableResolution = tableResolution;
    lfo.initialise([](float x) { return std::sin(x); }, tableResolution); // sine lfo
    lfo.setFrequency(frequency);
}

MorphingLfo::~MorphingLfo() {}

float MorphingLfo::getNextValue() {
    return lfo.processSample(0);
}


void MorphingLfo::setFrequency(float frequency) {
    jassert(frequency > 0);
    this->frequency = frequency;
    lfo.setFrequency(frequency);
}


//==============================================================================


void MorphingLfo::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;
    lfo.prepare({sampleRate, static_cast<uint32>(samplesPerBlock)});

    // random phase initialization
    int randPhase = Random::getSystemRandom().nextInt(tableResolution);
    for (int i = 0; i < randPhase; i++) {
        lfo.processSample(0.0);
    }
}

void MorphingLfo::releaseResources() {
    lfo.reset();
}
