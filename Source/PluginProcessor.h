#pragma once
#include "Params.h"

class LoopsChannelProcessor : public juce::AudioProcessor
{
public:
    LoopsChannelProcessor();
    ~LoopsChannelProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Loops Channel"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;

    // meters (linear peak), read by the editor
    std::atomic<float> inPeak[2], outPeak[2], grComp, grGate;

private:
    using Filt = juce::dsp::IIR::Filter<float>;
    using Coef = juce::dsp::IIR::Coefficients<float>;

    struct Chain
    {
        Filt hp1[2], hp2[2], lp[2], lfS[2], lmfP[2], hmfP[2], hfS[2];
        bool hpOn = false, lpOn = false;
        void reset();
        inline float filt (int ch, float x)
        {
            if (hpOn) { x = hp1[ch].processSample (x); x = hp2[ch].processSample (x); }
            if (lpOn) x = lp[ch].processSample (x);
            return x;
        }
        inline float eq (int ch, float x)
        {
            x = lfS[ch].processSample (x);
            x = lmfP[ch].processSample (x);
            x = hmfP[ch].processSample (x);
            return hfS[ch].processSample (x);
        }
    };

    void updateCoefs (double sr);
    float get (const char* id) const { return apvts.getRawParameterValue (id)->load(); }

    Chain audioChain, scChain;
    double sampleRate = 44100.0;
    float grC = 0.0f, grG = 0.0f;
    float lastKey[12] {};
    bool keyValid = false;
    int currentProgram = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopsChannelProcessor)
};
