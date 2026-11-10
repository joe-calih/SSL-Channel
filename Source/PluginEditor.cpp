#include "PluginEditor.h"

using namespace juce;
static Font fnt (float h, bool bold = true) { return Font (FontOptions (h, bold ? Font::bold : Font::plain)); }

//==============================================================================  look & feel
void LoopsLNF::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float sa, float ea, Slider& s)
{
    auto* k = dynamic_cast<Knob*> (&s);
    if (k == nullptr) return;
    const Point<float> c (x + w * 0.5f, y + h * 0.5f);
    const float r = k->dia * 0.5f, ang = sa + pos * (ea - sa);

    // tick marks + labels
    g.setFont (fnt (8.5f, false));
    for (auto& t : k->ticks)
    {
        const float a = sa + (float) k->valueToProportionOfLength (t.value) * (ea - sa);
        g.setColour (Colour (0xffd8d8d0));
        g.drawLine (Line<float> (c.getPointOnCircumference (r + 2.5f, a), c.getPointOnCircumference (r + 5.5f, a)), 1.0f);
        const auto p = c.getPointOnCircumference (r + 11.0f, a);
        if (t.text == "wide" || t.text == "narrow")
        {
            Path hat; const float hw = t.text == "wide" ? 5.5f : 2.5f, hh = 4.5f;
            hat.startNewSubPath (p.x - hw - 2, p.y + hh * 0.5f); hat.lineTo (p.x - hw * 0.5f, p.y - hh * 0.5f);
            hat.lineTo (p.x + hw * 0.5f, p.y - hh * 0.5f); hat.lineTo (p.x + hw + 2, p.y + hh * 0.5f);
            g.strokePath (hat, PathStrokeType (1.0f));
        }
        else g.drawText (t.text, Rectangle<float> (26, 10).withCentre (p), Justification::centred, false);
    }

    // shadow, skirt, cap
    g.setColour (Colours::black.withAlpha (0.55f));
    g.fillEllipse (c.x - r - 1.5f, c.y - r + 1.5f, 2 * r + 3, 2 * r + 3);
    g.setGradientFill (ColourGradient (Colour (0xff4a4a4a), c.x - r, c.y - r, Colour (0xff0b0b0b), c.x + r, c.y + r, false));
    g.fillEllipse (c.x - r, c.y - r, 2 * r, 2 * r);
    const float cr = r * 0.80f;
    g.setGradientFill (ColourGradient (k->cap.brighter (0.45f), c.x - cr * 0.7f, c.y - cr * 0.9f,
                                       k->cap.darker (0.55f), c.x + cr * 0.7f, c.y + cr, false));
    g.fillEllipse (c.x - cr, c.y - cr, 2 * cr, 2 * cr);
    g.setColour (Colours::black.withAlpha (0.7f));
    g.drawEllipse (c.x - cr, c.y - cr, 2 * cr, 2 * cr, 1.0f);
    g.setColour (Colours::white.withAlpha (0.18f));
    g.drawEllipse (c.x - cr + 1.5f, c.y - cr + 1.5f, 2 * cr - 3, 2 * cr - 3, 1.0f);

    // pointer
    g.setColour (k->lightCap ? Colour (0xff101010) : Colours::white);
    g.drawLine (Line<float> (c.getPointOnCircumference (cr * 0.22f, ang), c.getPointOnCircumference (cr * 0.96f, ang)), 2.2f);
}

void LoopsLNF::drawLinearSlider (Graphics& g, int x, int y, int w, int h, float pos, float, float, Slider::SliderStyle, Slider&)
{
    const float cx = x + w * 0.5f;
    g.setColour (Colour (0xff050505));
    g.fillRoundedRectangle (cx - 3, (float) y, 6, (float) h, 2.0f);
    g.setColour (Colour (0xff6c6c64));
    for (int i = 0; i <= 10; ++i)
        g.drawHorizontalLine ((int) (y + i * h / 10.0f), cx - 11, cx - 6);
    Rectangle<float> cap (cx - 13, pos - 8, 26, 16);
    g.setGradientFill (ColourGradient (Colour (0xffc8c8c0), cap.getX(), cap.getY(), Colour (0xff62625c), cap.getX(), cap.getBottom(), false));
    g.fillRoundedRectangle (cap, 2.0f);
    g.setColour (Colours::black); g.drawRoundedRectangle (cap, 2.0f, 1.0f);
    g.setColour (Colours::black.withAlpha (0.8f)); g.fillRect (cap.getX() + 3, cap.getCentreY() - 1, cap.getWidth() - 6, 2.0f);
}

void LoopsLNF::drawButtonBackground (Graphics& g, Button& b, const Colour& bg, bool, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (0.5f);
    const bool on = b.getToggleState();
    if (b.getProperties().getWithDefault ("switch", false))
    {
        g.setColour (Colour (0xff0a0a0a)); g.fillRoundedRectangle (r, 3.0f);
        g.setColour (Colour (0xff3a3a36)); g.drawRoundedRectangle (r, 3.0f, 1.0f);
        const float hw = r.getWidth() * 0.5f;
        Rectangle<float> knob (on ? r.getX() + 1 : r.getX() + hw, r.getY() + 1, hw - 1, r.getHeight() - 2);
        g.setColour (Colour (0xffb8b8b0)); g.fillRoundedRectangle (knob, 2.0f);
        return;
    }
    auto col = on ? bg.brighter (0.15f) : bg;
    if (down) col = col.darker (0.2f);
    g.setGradientFill (ColourGradient (col.brighter (0.25f), r.getX(), r.getY(), col.darker (0.25f), r.getX(), r.getBottom(), false));
    g.fillRoundedRectangle (r, 2.0f);
    g.setColour (Colours::black); g.drawRoundedRectangle (r, 2.0f, 1.0f);
    if (on) { g.setColour (Colours::black.withAlpha (0.25f)); g.fillRoundedRectangle (r.reduced (1), 2.0f); }
}

void LoopsLNF::drawButtonText (Graphics& g, TextButton& b, bool, bool)
{
    if (b.getProperties().getWithDefault ("switch", false)) return;
    g.setColour (b.findColour (TextButton::textColourOffId));
    g.setFont (fnt (b.getHeight() > 24 ? 8.5f : 9.5f));
    g.drawFittedText (b.getButtonText(), b.getLocalBounds().reduced (1), Justification::centred, 2, 0.8f);
}

//==============================================================================  editor
LoopsChannelEditor::LoopsChannelEditor (LoopsChannelProcessor& p) : AudioProcessorEditor (&p), proc (p)
{
    setLookAndFeel (&lnf);
    setSize (366, 678);
    setResizable (false, false);

    const Colour cream (0xffe9e6d8), red (0xffc0202c), green (0xff1d7a52), blue (0xff1b6ea3), black (0xff1a1a1a);
    const float pi = MathConstants<float>::pi;

    struct KD { const char* id; int cx, cy, dia; Colour col; bool light; std::vector<Tick> ticks; };
    const std::vector<KD> defs = {
        { ids::lpf, 142, 82, 40, cream, true, { {0,"OUT"}, {.51,"8"}, {.65,"6"}, {.74,"5"}, {.86,"4"}, {.92,"3.5"} } },
        { ids::hpf, 52, 124, 40, cream, true, { {20,"OUT"}, {70,"70"}, {120,"120"}, {200,"200"}, {300,"300"}, {350,"350"} } },
        { ids::hf, 52, 203, 38, red, false, {} },
        { ids::hfFreq, 142, 243, 36, red, false, { {1500,"1.5"}, {5000,"5"}, {8000,"8"}, {10000,"10"}, {14000,"14"}, {16000,"16"} } },
        { ids::hmf, 52, 285, 38, green, false, {} },
        { ids::hmfQ, 52, 362, 32, green, false, { {0.5,"wide"}, {3.0,"narrow"} } },
        { ids::hmfFreq, 142, 322, 36, green, false, { {600,".6"}, {1500,"1.5"}, {3000,"3"}, {4500,"4.5"}, {6000,"6"}, {7000,"7"} } },
        { ids::lmf, 52, 440, 38, blue, false, {} },
        { ids::lmfQ, 52, 518, 32, blue, false, { {0.5,"wide"}, {3.0,"narrow"} } },
        { ids::lmfFreq, 142, 486, 36, blue, false, { {200,".2"}, {500,".5"}, {1000,"1"}, {1500,"1.5"}, {2000,"2"}, {2500,"2.5"} } },
        { ids::lf, 52, 601, 38, black, false, {} },
        { ids::lfFreq, 142, 566, 36, black, false, { {30,"30"}, {50,"50"}, {100,"100"}, {200,"200"}, {300,"300"}, {450,"450"} } },
        { ids::ratio, 224, 82, 38, cream, true, { {1,"1"}, {2,"2"}, {4,"4"}, {10,"10"}, {20,"20"} } },
        { ids::cRel, 224, 160, 36, cream, true, { {0.05,".05"}, {0.1,".1"}, {0.2,".2"}, {0.4,".4"}, {1,"1"}, {3,"3"} } },
        { ids::cThresh, 316, 124, 38, cream, true, { {10,"10"}, {5,"5"}, {0,"0"}, {-5,"-5"}, {-10,"-10"}, {-15,"-15"}, {-20,"-20"} } },
        { ids::eThresh, 316, 204, 38, green, false, { {-40,"-40"}, {-30,"-30"}, {-20,"-20"}, {-10,"-10"}, {0,"0"} } },
        { ids::eRange, 224, 240, 38, green, false, { {0,"0"}, {10,"10"}, {20,"20"}, {30,"30"}, {40,"40"} } },
        { ids::eRel, 224, 324, 36, green, false, { {0.05,".05"}, {0.1,".1"}, {0.5,".5"}, {1,"1"}, {4,"4"} } },
        { ids::inGain, 231, 566, 36, cream, true, { {-18,"-18"}, {0,"0"}, {18,"+18"} } },
    };
    for (auto& d : defs)
    {
        auto k = std::make_unique<Knob> (d.col, d.dia, d.light, d.ticks);
        k->setSliderStyle (Slider::RotaryVerticalDrag);
        k->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        k->setRotaryParameters (pi * 1.2f, pi * 2.8f, true);
        k->setBounds (Rectangle<int> (92, 92).withCentre ({ d.cx, d.cy }));
        k->setPopupDisplayEnabled (true, true, this);
        addAndMakeVisible (*k);
        sAtt.push_back (std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, d.id, *k));
        if (auto* prm = proc.apvts.getParameter (d.id))
            k->setDoubleClickReturnValue (true, prm->convertFrom0to1 (prm->getDefaultValue()));
        knobs.push_back (std::move (k));
    }

    const uint32 grey = 0xffc9c8bf, teal = 0xff22544c, txtDark = 0xff111111, txtLight = 0xffeeeeee;
    addButton (ids::split,   "SPLIT",    145, 133, 40, 19, grey, txtDark);
    addButton (ids::hmfX3,   "HMF\xc3\x97" "3", 145, 188, 38, 19, teal, txtLight);
    addButton (ids::lmfD3,   "LMF\xc3\xb7" "3",    145, 611, 38, 19, teal, txtLight);
    addButton (ids::eqByp,   "BYP",      137, 401, 26, 52, grey, txtDark);
    addButton (ids::eqToSC,  "FLT DYN S.C.", 168, 401, 30, 52, grey, txtDark);
    addButton (ids::dynByp,  "BYP",      218, 401, 26, 52, grey, txtDark);
    addButton (ids::dynToOut, "CH. OUT", 248, 401, 30, 52, grey, txtDark);
    addButton (ids::gate,    "GATE",     316, 249, 40, 19, grey, txtDark);
    addButton (ids::phase,   "\xc3\x98", 231, 612, 40, 20, 0xffb8202a, txtLight);
    addButton (ids::cFast,   "", 237, 201, 24, 11, grey, txtDark, true);
    addButton (ids::eFast,   "", 237, 362, 24, 11, grey, txtDark, true);
    addButton (ids::analog,  "", 235, 474, 36, 12, grey, txtDark, true);

    for (auto* b : { &inputBtn, &outputBtn })
    {
        b->setColour (TextButton::buttonColourId, Colour (grey));
        b->setColour (TextButton::textColourOffId, Colour (txtDark));
        addAndMakeVisible (*b);
    }
    inputBtn.setBounds (Rectangle<int> (50, 18).withCentre ({ 316, 395 }));
    outputBtn.setBounds (Rectangle<int> (50, 18).withCentre ({ 316, 417 }));
    inputBtn.onClick  = [this] { proc.apvts.getParameter (ids::meterOut)->setValueNotifyingHost (0.0f); };
    outputBtn.onClick = [this] { proc.apvts.getParameter (ids::meterOut)->setValueNotifyingHost (1.0f); };

    fader.setBounds (Rectangle<int> (36, 170).withCentre ({ 345, 530 }));
    addAndMakeVisible (fader);
    faderAtt = std::make_unique<AudioProcessorValueTreeState::SliderAttachment> (proc.apvts, ids::outGain, fader);
    fader.setDoubleClickReturnValue (true, 0.0);
    fader.setPopupDisplayEnabled (true, true, this);

    for (auto* b : { &prevBtn, &nextBtn })
    {
        b->setColour (TextButton::buttonColourId, Colour (0xff3a3a3a));
        b->setColour (TextButton::textColourOffId, Colours::white);
        addAndMakeVisible (*b);
    }
    prevBtn.setBounds (4, 5, 28, 24); nextBtn.setBounds (34, 5, 28, 24);
    presetBox.setBounds (66, 5, 296, 24);
    for (int i = 0; i < proc.getNumPrograms(); ++i) presetBox.addItem (proc.getProgramName (i), i + 1);
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, dontSendNotification);
    presetBox.onChange = [this] { proc.setCurrentProgram (presetBox.getSelectedId() - 1); };
    addAndMakeVisible (presetBox);
    prevBtn.onClick = [this] { int i = (proc.getCurrentProgram() + proc.getNumPrograms() - 1) % proc.getNumPrograms();
                               presetBox.setSelectedId (i + 1, sendNotificationSync); };
    nextBtn.onClick = [this] { int i = (proc.getCurrentProgram() + 1) % proc.getNumPrograms();
                               presetBox.setSelectedId (i + 1, sendNotificationSync); };

    link.setBounds (6, 654, 120, 18);
    link.setFont (fnt (10.0f, false), false);
    link.setColour (HyperlinkButton::textColourId, Colour (0xff9fb8aa));
    addAndMakeVisible (link);

    startTimerHz (30);
}

LoopsChannelEditor::~LoopsChannelEditor() { setLookAndFeel (nullptr); }

TextButton& LoopsChannelEditor::addButton (const char* id, const char* text, int cx, int cy, int w, int h, uint32 col,
                                           uint32 textCol, bool sw)
{
    auto b = std::make_unique<TextButton>();
    b->setButtonText (String (CharPointer_UTF8 (text)));
    b->setClickingTogglesState (true);
    b->setColour (TextButton::buttonColourId, Colour (col));
    b->setColour (TextButton::textColourOffId, Colour (textCol));
    b->setColour (TextButton::textColourOnId, Colour (textCol));
    if (sw) b->getProperties().set ("switch", true);
    b->setBounds (Rectangle<int> (w, h).withCentre ({ cx, cy }));
    addAndMakeVisible (*b);
    bAtt.push_back (std::make_unique<AudioProcessorValueTreeState::ButtonAttachment> (proc.apvts, id, *b));
    buttons.push_back (std::move (b));
    return *buttons.back();
}

void LoopsChannelEditor::timerCallback()
{
    const bool outSel = param (ids::meterOut) > 0.5f;
    inputBtn.setToggleState (! outSel, dontSendNotification);
    outputBtn.setToggleState (outSel, dontSendNotification);
    auto& pk = outSel ? proc.outPeak : proc.inPeak;
    float mx = -100.0f;
    for (int c = 0; c < 2; ++c)
    {
        const float db = Decibels::gainToDecibels (pk[c].load(), -100.0f);
        meterDb[c] = db > meterDb[c] ? db : jmax (db, meterDb[c] - 2.5f);
        mx = jmax (mx, meterDb[c]);
    }
    peakHold = mx > peakHold ? mx : jmax (mx, peakHold - 0.15f);
    grC = proc.grComp.load();
    grG = proc.grGate.load();
    repaint();
}

void LoopsChannelEditor::led (Graphics& g, float cx, float cy, Colour c, bool on) const
{
    const float r = 3.5f;
    if (on)
    {
        g.setColour (c.withAlpha (0.35f)); g.fillEllipse (cx - r - 2, cy - r - 2, 2 * r + 4, 2 * r + 4);
        g.setGradientFill (ColourGradient (c.brighter (0.8f), cx - 1, cy - 1, c, cx + r, cy + r, true));
    }
    else g.setColour (c.darker (0.85f));
    g.fillEllipse (cx - r, cy - r, 2 * r, 2 * r);
    g.setColour (Colours::black.withAlpha (0.8f)); g.drawEllipse (cx - r, cy - r, 2 * r, 2 * r, 0.8f);
}

void LoopsChannelEditor::label (Graphics& g, const String& t, float cx, float cy, float size, bool bold) const
{
    g.setColour (Colour (0xffd9d7cc)); g.setFont (fnt (size, bold));
    g.drawText (t, Rectangle<float> (90, 14).withCentre ({ cx, cy }), Justification::centred, false);
}

void LoopsChannelEditor::paint (Graphics& g)
{
    // chassis
    g.setGradientFill (ColourGradient (Colour (0xff232321), 0, 0, Colour (0xff121211), 0, (float) getHeight(), false));
    g.fillAll();
    Random rnd (7);
    for (int i = 0; i < 2600; ++i)
    { g.setColour (Colours::white.withAlpha (rnd.nextFloat() * 0.035f)); g.fillRect (rnd.nextInt (366), rnd.nextInt (678), 1, 1); }

    // top bar
    g.setColour (Colour (0xff151515)); g.fillRect (0, 0, 366, 34);
    g.setColour (Colour (0xff3d3d3a)); g.drawHorizontalLine (34, 0, 366);

    // section frame lines
    g.setColour (Colour (0xff060606));
    auto line = [&g] (float x1, float y1, float x2, float y2)
    {
        g.setColour (Colours::black); g.drawLine (x1, y1, x2, y2, 1.5f);
        g.setColour (Colour (0xff4a4a46)); g.drawLine (x1 + 1, y1 + 1, x2 + 1, y2 + 1, 1.0f);
    };
    line (186, 62, 186, 650);
    line (10, 62, 186, 62); line (186, 62, 356, 62);
    line (10, 150, 80, 150); line (80, 150, 108, 176); line (108, 176, 186, 176);
    line (10, 270, 186, 270);
    line (10, 425, 120, 425); line (120, 425, 186, 425);
    line (10, 555, 120, 555);
    line (186, 178, 356, 178); line (186, 270, 356, 270);
    line (186, 520, 275, 520); line (275, 520, 275, 640); line (186, 640, 275, 640);
    line (275, 372, 275, 520);

    // headers
    label (g, "FILTERS", 62, 47, 11);
    label (g, "DYNAMICS", 311, 47, 11);
    label (g, "GAIN", 316, 374, 10);
    label (g, "ANALOG", 235, 453, 9);
    label (g, "IN", 231, 536, 8.5f);

    // captions: filters
    label (g, "kHz", 142, 107, 8.5f, false); label (g, "Hz", 52, 148, 8.5f, false);
    label (g, "dB", 52, 227, 8.5f, false); label (g, "dB", 52, 309, 8.5f, false);
    label (g, "dB", 52, 464, 8.5f, false); label (g, "dB", 52, 625, 8.5f, false);
    label (g, "kHz", 142, 263, 8.5f, false); label (g, "kHz", 142, 342, 8.5f, false);
    label (g, "kHz", 142, 507, 8.5f, false); label (g, "Hz", 142, 587, 8.5f, false);
    label (g, "HF", 96, 222, 11); label (g, "HMF", 96, 322, 11); label (g, "LMF", 96, 486, 11); label (g, "LF", 96, 583, 11);
    label (g, "EQ TO", 150, 372, 9.5f); label (g, "DYN TO", 232, 372, 9.5f);
    label (g, "ON", 205, 474, 8); label (g, "OFF", 266, 474, 8);
    // dynamics
    label (g, "RATIO", 224, 108, 8.5f); label (g, "COMPRESS", 224, 124, 10.5f); label (g, "RELEASE", 224, 188, 8.5f);
    label (g, "F.ATK", 205, 201, 7.5f); label (g, "F.ATK", 205, 362, 7.5f);
    label (g, "THRESHOLD", 316, 152, 8.5f); label (g, "THRESHOLD", 316, 232, 8.5f);
    label (g, "RANGE", 224, 267, 8.5f); label (g, "EXPAND", 224, 285, 10.5f); label (g, "RELEASE", 224, 352, 8.5f);
    label (g, "OUT", 340, 440, 8.5f);

    // EQ-TO / DYN-TO frame
    g.setColour (Colour (0xff6c6c66));
    g.drawRoundedRectangle (122.0f, 366.0f, 80.0f, 76.0f, 3.0f, 1.0f);
    g.drawRoundedRectangle (204.0f, 366.0f, 72.0f, 76.0f, 3.0f, 1.0f);

    // LEDs
    led (g, 109, 70, Colour (0xff3adf6a), param (ids::lpf) > 0.02f);
    led (g, 19, 109, Colour (0xff3adf6a), param (ids::hpf) > 20.5f);
    led (g, 173, 133, Colour (0xffff9b1a), param (ids::split) > 0.5f);
    led (g, 173, 188, Colour (0xff3adf6a), param (ids::hmfX3) > 0.5f);
    led (g, 173, 611, Colour (0xff3adf6a), param (ids::lmfD3) > 0.5f);
    led (g, 137, 435, Colour (0xffff3030), param (ids::eqByp) > 0.5f);
    led (g, 168, 435, Colour (0xffff9b1a), param (ids::eqToSC) > 0.5f);
    led (g, 218, 435, Colour (0xffff3030), param (ids::dynByp) > 0.5f);
    led (g, 248, 435, Colour (0xffff9b1a), param (ids::dynToOut) > 0.5f);
    led (g, 345, 250, Colour (0xff3adf6a), param (ids::gate) > 0.5f);
    led (g, 345, 395, Colour (0xffff9b1a), param (ids::meterOut) < 0.5f);
    led (g, 345, 417, Colour (0xffff9b1a), param (ids::meterOut) > 0.5f);
    led (g, 254, 612, Colour (0xffff3030), param (ids::phase) > 0.5f);

    // gain-reduction LED ladder
    g.setColour (Colour (0xff0b0b0a)); g.fillRoundedRectangle (288, 268, 58, 96, 3.0f);
    g.setColour (Colour (0xff55554f)); g.drawRoundedRectangle (288, 268, 58, 96, 3.0f, 1.0f);
    const int steps[5] = { 20, 14, 10, 6, 3 };
    for (int i = 0; i < 5; ++i)
    {
        const float y = 279.0f + i * 19.0f;
        led (g, 299, y, Colour (0xff3adf6a), grG >= steps[i] * (i == 4 ? 0.5f : 0.85f));
        led (g, 335, y, Colour (0xffff9b1a), grC >= steps[i] * (i == 4 ? 0.5f : 0.85f));
        g.setColour (Colour (0xffd9d7cc)); g.setFont (fnt (9.0f));
        g.drawText (String (steps[i]), Rectangle<float> (24, 12).withCentre ({ 317, y }), Justification::centred, false);
    }

    // level meters
    const int segs = 30;
    for (int c = 0; c < 2; ++c)
    {
        const float x = 303.0f + c * 17.0f;
        const int lit = jlimit (0, segs, (int) std::round ((meterDb[c] + 50.0f) / 50.0f * segs));
        for (int s = 0; s < segs; ++s)
        {
            const float y = 610.0f - (s + 1) * 5.7f;
            const float segDb = -50.0f + (s + 1) * 50.0f / segs;
            Colour col = segDb > -3.0f ? Colour (0xffe53935) : segDb > -12.0f ? Colour (0xffe9d41c) : Colour (0xff6fe02b);
            g.setColour (s < lit ? col : col.darker (0.88f));
            g.fillRect (x, y, 13.0f, 4.4f);
        }
    }
    const int mk[9] = { 0, 5, 10, 15, 20, 25, 30, 40, 50 };
    g.setColour (Colour (0xffd9d7cc)); g.setFont (fnt (8.5f, false));
    for (int m : mk)
        g.drawText (String (m), Rectangle<float> (22, 10).withCentre ({ 291.0f, 438.0f + m / 50.0f * 172.0f }), Justification::centredRight, false);
    g.setColour (Colour (0xff070707)); g.fillRoundedRectangle (296, 618, 40, 15, 2.0f);
    g.setColour (Colour (0xffe9e6d8)); g.setFont (fnt (11.0f));
    g.drawText (peakHold < -99.0f ? "-" : String (-peakHold, 1), Rectangle<float> (40, 15).translated (296, 618), Justification::centred, false);

    // analog sub-label & brand
    g.setColour (Colour (0xff8a8a82)); g.setFont (fnt (9.5f, false));
    g.drawText ("Loops Channel  \xc2\xb7  v1.0", Rectangle<int> (130, 654, 226, 18), Justification::centredRight, false);
}
