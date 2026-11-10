#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>

namespace ids
{
    inline constexpr const char* hpf = "hpf";        inline constexpr const char* lpf = "lpf";
    inline constexpr const char* split = "split";
    inline constexpr const char* hf = "hf";          inline constexpr const char* hfFreq = "hfFreq";
    inline constexpr const char* hmf = "hmf";        inline constexpr const char* hmfFreq = "hmfFreq";
    inline constexpr const char* hmfQ = "hmfQ";      inline constexpr const char* hmfX3 = "hmfX3";
    inline constexpr const char* lmf = "lmf";        inline constexpr const char* lmfFreq = "lmfFreq";
    inline constexpr const char* lmfQ = "lmfQ";      inline constexpr const char* lmfD3 = "lmfD3";
    inline constexpr const char* lf = "lf";          inline constexpr const char* lfFreq = "lfFreq";
    inline constexpr const char* eqByp = "eqByp";    inline constexpr const char* eqToSC = "eqToSC";
    inline constexpr const char* dynByp = "dynByp";  inline constexpr const char* dynToOut = "dynToOut";
    inline constexpr const char* ratio = "ratio";    inline constexpr const char* cThresh = "cThresh";
    inline constexpr const char* cRel = "cRel";      inline constexpr const char* cFast = "cFast";
    inline constexpr const char* eRange = "eRange";  inline constexpr const char* eThresh = "eThresh";
    inline constexpr const char* eRel = "eRel";      inline constexpr const char* eFast = "eFast";
    inline constexpr const char* gate = "gate";      inline constexpr const char* analog = "analog";
    inline constexpr const char* inGain = "inGain";  inline constexpr const char* phase = "phase";
    inline constexpr const char* outGain = "outGain"; inline constexpr const char* meterOut = "meterOut";
}

// LPF knob is 0..1 (0 = OUT, 1 = 3 kHz), geometric mapping
inline float lpfKnobToHz (float k) { return 22000.0f * std::pow (3000.0f / 22000.0f, k); }
