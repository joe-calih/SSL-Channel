#include "PluginProcessor.h"
#include "PluginEditor.h"

using Layout = juce::AudioProcessorValueTreeState::ParameterLayout;

//==============================================================================
Layout LoopsChannelProcessor::createLayout()
{
    Layout l;
    using Attr = juce::AudioParameterFloatAttributes;

    auto addF = [&] (const char* id, const char* name, float lo, float hi, float def, float centre, const char* unit)
    {
        juce::NormalisableRange<float> r (lo, hi);
        if (centre > 0.0f) r.setSkewForCentre (centre);
        l.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, r, def,
                                                            Attr().withLabel (unit)));
    };
    auto addB = [&] (const char* id, const char* name, bool def)
    {
        l.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { id, 1 }, name, def));
    };

    addF (ids::hpf, "HPF", 20.0f, 350.0f, 20.0f, 70.0f, "Hz");
    l.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { ids::lpf, 1 }, "LPF",
            juce::NormalisableRange<float> (0.0f, 1.0f), 0.0f,
            Attr().withStringFromValueFunction ([] (float v, int) {
                return v < 0.02f ? juce::String ("OUT") : juce::String (lpfKnobToHz (v) / 1000.0f, 2) + " kHz"; })));
    addB (ids::split, "Filter Split", false);

    addF (ids::hf,    "HF Gain", -16.0f, 16.0f, 0.0f, 0.0f, "dB");
    addF (ids::hfFreq, "HF Freq", 1500.0f, 16000.0f, 8000.0f, 5000.0f, "Hz");
    addF (ids::hmf,   "HMF Gain", -16.0f, 16.0f, 0.0f, 0.0f, "dB");
    addF (ids::hmfFreq, "HMF Freq", 600.0f, 7000.0f, 3000.0f, 2000.0f, "Hz");
    addF (ids::hmfQ,  "HMF Q", 0.5f, 3.0f, 1.0f, 1.2f, "");
    addB (ids::hmfX3, "HMF x3", false);
    addF (ids::lmf,   "LMF Gain", -16.0f, 16.0f, 0.0f, 0.0f, "dB");
    addF (ids::lmfFreq, "LMF Freq", 200.0f, 2500.0f, 800.0f, 700.0f, "Hz");
    addF (ids::lmfQ,  "LMF Q", 0.5f, 3.0f, 1.0f, 1.2f, "");
    addB (ids::lmfD3, "LMF /3", false);
    addF (ids::lf,    "LF Gain", -16.0f, 16.0f, 0.0f, 0.0f, "dB");
    addF (ids::lfFreq, "LF Freq", 30.0f, 450.0f, 100.0f, 100.0f, "Hz");

    addB (ids::eqByp, "EQ Bypass", false);
    addB (ids::eqToSC, "EQ to Dyn S.C.", false);
    addB (ids::dynByp, "Dyn Bypass", false);
    addB (ids::dynToOut, "Dyn to Ch. Out", false);

    addF (ids::ratio, "Ratio", 1.0f, 20.0f, 1.0f, 4.0f, ":1");
    addF (ids::cThresh, "Comp Threshold", -20.0f, 10.0f, 10.0f, 0.0f, "dB");
    addF (ids::cRel, "Comp Release", 0.05f, 3.0f, 0.4f, 0.4f, "s");
    addB (ids::cFast, "Comp Fast Attack", false);
    addF (ids::eRange, "Range", 0.0f, 40.0f, 0.0f, 0.0f, "dB");
    addF (ids::eThresh, "Exp Threshold", -40.0f, 0.0f, -40.0f, 0.0f, "dB");
    addF (ids::eRel, "Exp Release", 0.05f, 4.0f, 0.3f, 0.5f, "s");
    addB (ids::eFast, "Exp Fast Attack", false);
    addB (ids::gate, "Gate", false);

    addB (ids::analog, "Analog", true);
    addF (ids::inGain, "Input", -18.0f, 18.0f, 0.0f, 0.0f, "dB");
    addB (ids::phase, "Phase", false);
    addF (ids::outGain, "Output", -40.0f, 12.0f, 0.0f, 0.0f, "dB");
    addB (ids::meterOut, "Meter Output", true);
    return l;
}

//==============================================================================
namespace
{
    struct Preset { const char* name; std::vector<std::pair<const char*, float>> v; };

    // lpf values are knob positions: 0 = OUT, .4 ~ 10k, .6 ~ 6.8k, .8 ~ 4.4k
    const std::vector<Preset>& presets()
    {
        using namespace ids;
        static const std::vector<Preset> p = {
            { "Default", {} },
            { "Vocal - Presence & Air", { {hpf,90}, {hf,3.5f}, {hfFreq,10000}, {hmf,2.5f}, {hmfFreq,3500}, {hmfQ,1.2f},
                                         {lmf,-2}, {lmfFreq,350}, {ratio,3}, {cThresh,-12}, {cRel,0.35f} } },
            { "Vocal - Smooth Leveler", { {hpf,80}, {hf,2}, {hfFreq,12000}, {hmf,1.5f}, {hmfFreq,2800}, {lmf,-1.5f}, {lmfFreq,400},
                                         {ratio,4}, {cThresh,-14}, {cRel,0.5f} } },
            { "Vocal - Rap Forward", { {hpf,110}, {hf,4}, {hfFreq,8000}, {hmf,3}, {hmfFreq,4500}, {lmf,-3}, {lmfFreq,300}, {lmfQ,1.5f},
                                      {ratio,4}, {cThresh,-16}, {cFast,1}, {cRel,0.2f} } },
            { "Podcast Voice", { {hpf,100}, {lpf,0.4f}, {lmf,-2}, {lmfFreq,250}, {hmf,3}, {hmfFreq,3000}, {ratio,4}, {cThresh,-16},
                                {cRel,0.3f}, {eThresh,-34}, {eRange,12}, {eRel,0.3f} } },
            { "Drum Bus - Glue", { {hpf,30}, {lf,2}, {lfFreq,60}, {hf,2}, {hfFreq,10000}, {ratio,2}, {cThresh,-10}, {cRel,0.1f} } },
            { "Kick - Punch", { {hpf,25}, {lf,4}, {lfFreq,60}, {lmf,-4}, {lmfFreq,400}, {lmfQ,1.5f}, {hmf,3}, {hmfFreq,3500}, {hmfQ,1.2f},
                               {ratio,4}, {cThresh,-10}, {cRel,0.15f} } },
            { "Snare - Crack", { {hpf,80}, {lf,2}, {lfFreq,200}, {lmf,-2}, {lmfFreq,500}, {hmf,3}, {hmfFreq,5000}, {hf,2}, {hfFreq,10000},
                                {ratio,4}, {cThresh,-12}, {cFast,1}, {cRel,0.1f} } },
            { "Gate - Tight Toms", { {hpf,60}, {lf,3}, {lfFreq,100}, {lmf,-3}, {lmfFreq,350}, {hmf,2}, {hmfFreq,3500}, {ratio,3}, {cThresh,-8},
                                    {eThresh,-25}, {eRange,25}, {gate,1}, {eRel,0.15f}, {eFast,1} } },
            { "Expand - Room Mics", { {hf,3}, {hfFreq,10000}, {ratio,4}, {cThresh,-18}, {cRel,0.2f}, {eThresh,-32}, {eRange,8}, {eRel,0.5f} } },
            { "Bass - Tight & Round", { {hpf,30}, {lpf,0.55f}, {lf,3}, {lfFreq,80}, {lmf,-3}, {lmfFreq,250}, {hmf,2}, {hmfFreq,1500},
                                       {ratio,4}, {cThresh,-14}, {cRel,0.3f} } },
            { "Guitar - Bite", { {hpf,100}, {lmf,-2.5f}, {lmfFreq,400}, {hmf,3}, {hmfFreq,3000}, {hf,2}, {hfFreq,8000}, {ratio,3}, {cThresh,-10} } },
            { "Acoustic - Sparkle", { {hpf,80}, {lf,1}, {lmf,-2}, {lmfFreq,250}, {hf,4}, {hfFreq,12000}, {hmf,1.5f}, {hmfFreq,4000},
                                     {ratio,2.5f}, {cThresh,-12} } },
            { "Piano - Warm", { {hpf,40}, {lf,2}, {lfFreq,100}, {lmf,-1.5f}, {lmfFreq,500}, {hf,2}, {hfFreq,10000}, {ratio,2}, {cThresh,-10} } },
            { "Mix Bus - Glue", { {hpf,25}, {lf,1}, {lfFreq,60}, {hf,1.5f}, {hfFreq,14000}, {ratio,2}, {cThresh,-6}, {cRel,0.3f} } },
            { "Master - Open Sparkle", { {lf,1.5f}, {lfFreq,50}, {lmf,-1}, {lmfFreq,300}, {hf,2.5f}, {hfFreq,14000}, {ratio,1.5f}, {cThresh,-4}, {cRel,0.4f} } },
            { "De-Mud Clean-Up", { {hpf,70}, {lmf,-4}, {lmfFreq,300}, {hmf,1}, {hmfFreq,3000} } },
            { "Parallel Squash", { {hf,3}, {hfFreq,10000}, {ratio,10}, {cThresh,-20}, {cFast,1}, {cRel,0.1f} } },
            { "Lo-Fi Telephone", { {hpf,300}, {lpf,0.8f}, {hmf,5}, {hmfFreq,2000}, {ratio,6}, {cThresh,-14} } },
        };
        return p;
    }
}

//==============================================================================
void LoopsChannelProcessor::Chain::reset()
{
    for (int c = 0; c < 2; ++c)
    {
        hp1[c].reset(); hp2[c].reset(); lp[c].reset(); lfS[c].reset(); lmfP[c].reset(); hmfP[c].reset(); hfS[c].reset();
    }
}

LoopsChannelProcessor::LoopsChannelProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    for (int c = 0; c < 2; ++c) { inPeak[c] = 0.0f; outPeak[c] = 0.0f; }
    grComp = 0.0f; grGate = 0.0f;
}

bool LoopsChannelProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    auto out = l.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo()) && out == l.getMainInputChannelSet();
}

void LoopsChannelProcessor::prepareToPlay (double sr, int)
{
    sampleRate = sr;
    audioChain.reset(); scChain.reset();
    grC = grG = 0.0f;
    keyValid = false;
}

void LoopsChannelProcessor::updateCoefs (double sr)
{
    const float hpfV = get (ids::hpf), lpfK = get (ids::lpf);
    const float hmfF = get (ids::hmfFreq) * (get (ids::hmfX3) > 0.5f ? 3.0f : 1.0f);
    const float lmfF = get (ids::lmfFreq) / (get (ids::lmfD3) > 0.5f ? 3.0f : 1.0f);
    const float key[12] = { hpfV, lpfK, get (ids::hf), get (ids::hfFreq), get (ids::hmf), get (ids::hmfQ),
                            hmfF, get (ids::lmf), get (ids::lmfQ), lmfF, get (ids::lf), get (ids::lfFreq) };

    if (keyValid && std::memcmp (key, lastKey, sizeof key) == 0) return;
    std::memcpy (lastKey, key, sizeof key);
    keyValid = true;

    const float nyq = (float) sr * 0.45f;
    auto fc = [nyq] (float f) { return juce::jlimit (20.0f, nyq, f); };
    auto g = [] (float db) { return juce::Decibels::decibelsToGain (db); };

    const bool hpOn = hpfV > 20.5f, lpOn = lpfK > 0.02f;
    auto hp1 = Coef::makeFirstOrderHighPass (sr, fc (hpfV));
    auto hp2 = Coef::makeHighPass (sr, fc (hpfV), 0.707f);
    auto lp  = Coef::makeLowPass (sr, fc (lpfKnobToHz (lpfK)), 0.707f);
    auto lfc = Coef::makeLowShelf (sr, fc (get (ids::lfFreq)), 0.7f, g (get (ids::lf)));
    auto lmc = Coef::makePeakFilter (sr, fc (lmfF), get (ids::lmfQ), g (get (ids::lmf)));
    auto hmc = Coef::makePeakFilter (sr, fc (hmfF), get (ids::hmfQ), g (get (ids::hmf)));
    auto hfc = Coef::makeHighShelf (sr, fc (get (ids::hfFreq)), 0.7f, g (get (ids::hf)));

    for (auto* ch : { &audioChain, &scChain })
    {
        ch->hpOn = hpOn; ch->lpOn = lpOn;
        for (int c = 0; c < 2; ++c)
        {
            ch->hp1[c].coefficients = hp1; ch->hp2[c].coefficients = hp2; ch->lp[c].coefficients = lp;
            ch->lfS[c].coefficients = lfc; ch->lmfP[c].coefficients = lmc;
            ch->hmfP[c].coefficients = hmc; ch->hfS[c].coefficients = hfc;
        }
    }
}

void LoopsChannelProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    const int nCh = juce::jmin (2, buffer.getNumChannels());
    if (nCh == 0) return;

    updateCoefs (sampleRate);

    const bool split = get (ids::split) > 0.5f, eqByp = get (ids::eqByp) > 0.5f, dynByp = get (ids::dynByp) > 0.5f;
    const bool scUseEq = ! eqByp && (get (ids::eqToSC) > 0.5f || get (ids::dynToOut) > 0.5f);
    const bool analog = get (ids::analog) > 0.5f, gateOn = get (ids::gate) > 0.5f;
    const float inG = juce::Decibels::decibelsToGain (get (ids::inGain)) * (get (ids::phase) > 0.5f ? -1.0f : 1.0f);
    const float outG = juce::Decibels::decibelsToGain (get (ids::outGain), -60.0f);

    const float ratio = get (ids::ratio), cT = get (ids::cThresh);
    const float cSlope = ratio >= 19.5f ? 1.0f : 1.0f - 1.0f / ratio;
    const bool cActive = ! dynByp && ratio > 1.01f;
    const float eRange = get (ids::eRange), eT = get (ids::eThresh);
    const float eSlope = gateOn ? 9.0f : 1.0f;      // gate ~10:1, expander 2:1
    const bool eActive = ! dynByp && eRange > 0.01f;

    auto coef = [this] (float tau) { return 1.0f - std::exp (-1.0f / (juce::jmax (1.0e-5f, tau) * (float) sampleRate)); };
    const float cAtk = coef (get (ids::cFast) > 0.5f ? 0.0001f : 0.003f), cRelC = coef (get (ids::cRel) * 0.5f);
    const float eAtk = coef (get (ids::eFast) > 0.5f ? 0.0001f : 0.0015f), eRelC = coef (get (ids::eRel) * 0.5f);

    float* data[2] = { buffer.getWritePointer (0), nCh > 1 ? buffer.getWritePointer (1) : nullptr };
    float inMax[2] = { 0, 0 }, outMax[2] = { 0, 0 }, grCMax = 0, grGMax = 0;

    for (int i = 0; i < n; ++i)
    {
        float v[2] = { 0, 0 }, s[2] = { 0, 0 };
        for (int c = 0; c < nCh; ++c)
        {
            const float x = data[c][i] * inG;
            inMax[c] = juce::jmax (inMax[c], std::abs (x));
            float a = x, sc;
            if (split) sc = scChain.filt (c, x);
            else { a = audioChain.filt (c, x); sc = a; }
            v[c] = eqByp ? a : audioChain.eq (c, a);
            s[c] = scUseEq ? scChain.eq (c, sc) : sc;
        }

        const float lvl = juce::jmax (std::abs (s[0]), std::abs (s[1]));
        const float db = juce::Decibels::gainToDecibels (lvl, -100.0f);
        const float cTarget = cActive ? juce::jmax (0.0f, (db - cT) * cSlope) : 0.0f;
        const float eTarget = eActive ? juce::jmin (eRange, juce::jmax (0.0f, (eT - db) * eSlope)) : 0.0f;
        grC += (cTarget > grC ? cAtk : cRelC) * (cTarget - grC);
        grG += (eTarget < grG ? eAtk : eRelC) * (eTarget - grG);
        grCMax = juce::jmax (grCMax, grC); grGMax = juce::jmax (grGMax, grG);
        const float gain = juce::Decibels::decibelsToGain (-(grC + grG));

        for (int c = 0; c < nCh; ++c)
        {
            float y = v[c] * gain;
            if (analog) y = std::tanh (y * 1.15f) / 1.15f;
            y *= outG;
            outMax[c] = juce::jmax (outMax[c], std::abs (y));
            data[c][i] = y;
        }
    }

    const float decay = std::exp (-(float) n / ((float) sampleRate * 0.35f));
    for (int c = 0; c < nCh; ++c)
    {
        inPeak[c]  = juce::jmax (inMax[c],  inPeak[c].load()  * decay);
        outPeak[c] = juce::jmax (outMax[c], outPeak[c].load() * decay);
    }
    if (nCh == 1) { inPeak[1] = inPeak[0].load(); outPeak[1] = outPeak[0].load(); }
    grComp = grCMax; grGate = grGMax;
}

//==============================================================================
int LoopsChannelProcessor::getNumPrograms() { return (int) presets().size(); }
const juce::String LoopsChannelProcessor::getProgramName (int i)
{
    return juce::isPositiveAndBelow (i, getNumPrograms()) ? juce::String (presets()[(size_t) i].name) : juce::String();
}

void LoopsChannelProcessor::setCurrentProgram (int index)
{
    if (! juce::isPositiveAndBelow (index, getNumPrograms())) return;
    currentProgram = index;
    for (auto* p : getParameters())
        if (auto* r = dynamic_cast<juce::RangedAudioParameter*> (p))
        {
            const auto id = r->getParameterID();
            if (id == ids::outGain || id == ids::meterOut || id == ids::inGain || id == ids::phase) continue;
            r->setValueNotifyingHost (r->getDefaultValue());
        }
    for (auto& kv : presets()[(size_t) index].v)
        if (auto* r = apvts.getParameter (kv.first))
            r->setValueNotifyingHost (r->convertTo0to1 (kv.second));
}

void LoopsChannelProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    state.setProperty ("program", currentProgram, nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary (*xml, dest);
}

void LoopsChannelProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            currentProgram = (int) tree.getProperty ("program", 0);
            apvts.replaceState (tree);
        }
}

juce::AudioProcessorEditor* LoopsChannelProcessor::createEditor() { return new LoopsChannelEditor (*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new LoopsChannelProcessor(); }
