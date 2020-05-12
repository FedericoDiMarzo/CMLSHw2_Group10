/*
  ==============================================================================

    Utils.h
    Created: 12 May 2020 6:15:30pm
    Author:  Federico Di Marzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Utils {
    float interpolate(float start, float end, float fract) {
        return start + (end - start) * fract;
    }
}