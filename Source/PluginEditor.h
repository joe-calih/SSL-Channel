#pragma once
#include "PluginProcessor.h"

struct Tick { double value; juce::String text; };

class Knob : public juce::Slider
{
public:
    Knob (juce::Colour capColour, int diameter, bool lightCapIn, std::vector<Tick> t)
        : cap (capColour), dia (diameter), lightCap (lightCapIn), ticks (std::move (t)) {}
    bool hitTest (int x, int y) override
    {
        return juce::Point<int> (x, y).getDistanceFrom (getLocalBounds().getCentre()) < dia / 2 + 6;
    }
    juce::Colour cap; int dia; bool lightCap; std::vector<Tick> ticks;
};

class LoopsLNF : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float startAngle, float endAngle, juce::Slider&) override;
    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h, float sliderPos, float minPos, float maxPos,
                           juce::Slider::SliderStyle, juce::Slider&) override;
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool) override;
    void drawButtonText (juce::Graphics&, juce::TextButton&, bool, bool) override;
};

class LoopsChannelEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit LoopsChannelEditor (LoopsChannelProcessor&);
    ~LoopsChannelEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override {}

private:
    void timerCallback() override;
    float param (const char* id) const { return proc.apvts.getRawParameterValue (id)->load(); }
    void led (juce::Graphics&, float cx, float cy, juce::Colour c, bool on) const;
    void label (juce::Graphics&, const juce::String&, float cx, float cy, float size = 9.0f, bool bold = true) const;
    juce::TextButton& addButton (const char* id, const char* text, int cx, int cy, int w, int h, juce::uint32 col,
                                 juce::uint32 textCol, bool sw = false);

    LoopsChannelProcessor& proc;
    LoopsLNF lnf;

    std::vector<std::unique_ptr<Knob>> knobs;
    std::vector<std::unique_ptr<juce::TextButton>> buttons;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sAtt;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> bAtt;

    juce::Slider fader { juce::Slider::LinearVertical, juce::Slider::NoTextBox };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> faderAtt;
    juce::TextButton prevBtn { "<" }, nextBtn { ">" }, inputBtn { "INPUT" }, outputBtn { "OUTPUT" };
    juce::ComboBox presetBox;
    juce::HyperlinkButton link { "loops.co.ke", juce::URL ("https://loops.co.ke") };

    float meterDb[2] { -100, -100 }, peakHold = -100.0f;
    float grC = 0, grG = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopsChannelEditor)
};
