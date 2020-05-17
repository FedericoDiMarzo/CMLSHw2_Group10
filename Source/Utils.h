
#pragma once

#include <JuceHeader.h>

namespace utils {
    /**
     * Linear interpolation.
     *
     * @param start
     * @param end
     * @param fract
     * @return interpolated value
     */
    inline float interpolate(float start, float end, float fract) {
        return start + (end - start) * fract;
    }
    /**
     * Encodes a L-R buffer into an S-M buffer.
     *
     * @param buffer
     */
    inline void midSideEncoding(AudioBuffer<float> &buffer) {
        int samplesForBlock = buffer.getNumSamples();
        AudioBuffer<float> tmp(buffer);

        // side (L-R)
        buffer.addFrom(0, 0, buffer,
                       1, 0, samplesForBlock, -1);

        // mid (L+R)
        buffer.addFrom(1, 0, tmp,
                       0, 0, samplesForBlock);

    }

    /**
     * Decodes a S-M buffer into an L-R buffer.
     *
     * @param buffer
     */
    inline void midSideDecoding(AudioBuffer<float> &buffer) {
        int samplesForBlock = buffer.getNumSamples();
        AudioBuffer<float> tmp(buffer);

        // left (M+S)/2
        buffer.addFrom(0, 0, buffer,
                       1, 0, samplesForBlock);

        // right (M-S)/2
        buffer.addFrom(1, 0, tmp,
                       0, 0, samplesForBlock);

        buffer.applyGain(0.5);
    }

    /**
     * Applies mid-side enhancement to the stereo buffer.
     * @param buffer
     * @param enhancing gain of the side channel.
     */
    inline void enhanceStereo(AudioBuffer<float> &buffer, float enhancing) {
        jassert(buffer.getNumChannels() == 2);
        jassert(enhancing >= 0);
        jassert(enhancing <= 1);

        int samplesForBlock = buffer.getNumSamples();

        // processing the side signal
        midSideEncoding(buffer);
        buffer.applyGain(0, 0, samplesForBlock, enhancing);

        // reconverting the buffer
        midSideDecoding(buffer);
    }
}

