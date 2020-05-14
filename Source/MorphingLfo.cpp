/*
  ==============================================================================

    MorphingLfo.cpp
    Created: 14 May 2020 9:45:39pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#include "MorphingLfo.h"

MorphingLfo::MorphingLfo(int morphingSteps, int tableResolution) {
    for (int i = 0; i < morphingSteps; i++) {
        lfoPool.push_back(std::make_unique<dsp::Oscillator<float>>());
        lfoPool[i]->initialise([](float x) {return std::sin(x);}, tableResolution); // sine lfo
    }
}

MorphingLfo::~MorphingLfo() {}

float MorphingLfo::getNextValue() {
    return lfoPool[0]->processSample(0);
}

//==============================================================================


void MorphingLfo::prepareToPlay(double sampleRate, int samplesPerBlock) {
    this->sampleRate = sampleRate;
    for (auto &lfo : lfoPool) {
        lfo->prepare({sampleRate, static_cast<uint32>(samplesPerBlock)});
    }
}

void MorphingLfo::releaseResources() {
    for (auto &lfo : lfoPool) {
        lfo->reset();
    }
}
