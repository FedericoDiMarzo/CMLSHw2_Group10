/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Cmls_hw2_group10AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Cmls_hw2_group10AudioProcessorEditor (Cmls_hw2_group10AudioProcessor&);
    ~Cmls_hw2_group10AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Cmls_hw2_group10AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Cmls_hw2_group10AudioProcessorEditor)
};
