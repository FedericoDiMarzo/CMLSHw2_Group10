
#pragma once
#include <JuceHeader.h>

namespace utils {
    float interpolate(float start, float end, float fract) {
        return start + (end - start) * fract;
    }


}

