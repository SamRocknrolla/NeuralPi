/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UdpRcServer.h"
#include <stdio.h>
#include <fstream>
#include <iostream>


//==============================================================================
NeuralPiAudioProcessorEditor::NeuralPiAudioProcessorEditor (NeuralPiAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
    , m_rcSrv(NpRpcProto::NPRPC_SRV_PORT, NpRpcProto::NPRPC_MCAST_ADDR, *this)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to

    blueLookAndFeel.setColour(juce::Slider::thumbColourId, juce::Colours::aqua);
    redLookAndFeel.setColour(juce::Slider::thumbColourId, juce::Colours::red);

    //addAndMakeVisible(modelKnob);
    //ampGainKnob.setLookAndFeel(&ampSilverKnobLAF);
    modelKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    modelKnob.setNumDecimalPlacesToDisplay(1);
    modelKnob.addListener(this);
    //modelKnob.setRange(0, processor.jsonFiles.size() - 1);
    modelKnob.setRange(0.0, 1.0);
    modelKnob.setValue(0.0);
    modelKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    modelKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    modelKnob.setNumDecimalPlacesToDisplay(1);
    modelKnob.setDoubleClickReturnValue(true, 0.0);

    auto modelValue = getParameterValue(modelName);
    Slider& modelSlider = getModelSlider();
    modelSlider.setValue(modelValue, NotificationType::dontSendNotification);

    modelKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getModelSlider().getValue());
        const float modelValue = getParameterValue(modelName);

        if (!approximatelyEqual(modelValue, sliderValue))
        {
            setParameterValue(modelName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getModelSlider().getValue());
        }
    };
    
    
    addAndMakeVisible(modelSelect);
    modelSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int c = 1;
    for (const auto& jsonFile : processor.jsonFiles) {
        modelSelect.addItem(jsonFile.getFileNameWithoutExtension(), c);
        c += 1;
    }
    modelSelect.onChange = [this] { 
        int index = modelSelect.getSelectedItemIndex();
        modelSelectChanged(index);
        m_rcSrv.updateModelIndex(static_cast<int32_t>(NpRpcProto::EComboBoxId::Model), index);
    };
    modelSelect.setSelectedItemIndex(processor.current_model_index, juce::NotificationType::dontSendNotification);
    modelSelect.setScrollWheelEnabled(true);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Import Tone");
    loadButton.setColour(juce::Label::textColourId, juce::Colours::black);
    loadButton.addListener(this);


    //addAndMakeVisible(irKnob);
    //irKnob.setLookAndFeel(&ampSilverKnobLAF);
    irKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    irKnob.setNumDecimalPlacesToDisplay(1);
    irKnob.addListener(this);
    //irKnob.setRange(0, processor.irFiles.size() - 1);
    irKnob.setRange(0.0, 1.0);
    irKnob.setValue(0.0);
    irKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    irKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20);
    irKnob.setNumDecimalPlacesToDisplay(1);
    irKnob.setDoubleClickReturnValue(true, 0.0);

    auto irValue = getParameterValue(irName);
    Slider& irSlider = getIrSlider();
    irSlider.setValue(irValue, NotificationType::dontSendNotification);

    irKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getIrSlider().getValue());
        const float irValue = getParameterValue(irName);

        if (!approximatelyEqual(irValue, sliderValue))
        {
            setParameterValue(irName, sliderValue);
        }
    };

    addAndMakeVisible(irSelect);
    irSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int i = 1;
    for (const auto& jsonFile : processor.irFiles) {
        irSelect.addItem(jsonFile.getFileNameWithoutExtension(), i);
        i += 1;
    }
    irSelect.onChange = [this] {
        int index = irSelect.getSelectedItemIndex();
        irSelectChanged(index);
        m_rcSrv.updateModelIndex(static_cast<int32_t>(NpRpcProto::EComboBoxId::Ir), index);
    };
    irSelect.setSelectedItemIndex(processor.current_ir_index, juce::NotificationType::dontSendNotification);
    irSelect.setScrollWheelEnabled(true);

    addAndMakeVisible(loadIR);
    loadIR.setButtonText("Import IR");
    loadIR.setColour(juce::Label::textColourId, juce::Colours::black);
    loadIR.addListener(this);

    // Toggle IR
    //addAndMakeVisible(irButton); // Toggle is for testing purposes
    irButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    irButton.onClick = [this] { updateToggleState(&irButton, "IR");   };

    // Toggle LSTM
    //addAndMakeVisible(lstmButton); // Toggle is for testing purposes
    lstmButton.setToggleState(true, juce::NotificationType::dontSendNotification);
    lstmButton.onClick = [this] { updateToggleState(&lstmButton, "LSTM");   };
  

    addAndMakeVisible(ampGainKnob);
    ampGainKnob.setLookAndFeel(&blueLookAndFeel);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampGainKnob.setNumDecimalPlacesToDisplay(1);
    ampGainKnob.addListener(this);
    ampGainKnob.setRange(0.0, 1.0);
    ampGainKnob.setValue(0.5);
    ampGainKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampGainKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampGainKnob.setNumDecimalPlacesToDisplay(2);
    ampGainKnob.setDoubleClickReturnValue(true, 0.5);

    auto gainValue = getParameterValue(gainName);
    Slider& gainSlider = getGainSlider();
    gainSlider.setValue(gainValue, NotificationType::dontSendNotification);

    ampGainKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getGainSlider().getValue());
        const float gainValue = getParameterValue(gainName);

        if (!approximatelyEqual(gainValue, sliderValue))
        {
            setParameterValue(gainName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getGainSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Gain), value);
        }
    };

    addAndMakeVisible(ampMasterKnob);
    ampMasterKnob.setLookAndFeel(&blueLookAndFeel);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.addListener(this);
    ampMasterKnob.setRange(0.0, 1.0);
    ampMasterKnob.setValue(0.5);
    ampMasterKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMasterKnob.setNumDecimalPlacesToDisplay(2);
    //ampMasterKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, false, 50, 20 );
    //ampMasterKnob.setNumDecimalPlacesToDisplay(1);
    ampMasterKnob.setDoubleClickReturnValue(true, 0.5);

    auto masterValue = getParameterValue(masterName);
    Slider& masterSlider = getMasterSlider();
    masterSlider.setValue(masterValue, NotificationType::dontSendNotification);

    ampMasterKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getMasterSlider().getValue());
        const float masterValue = getParameterValue(masterName);

        if (!approximatelyEqual(masterValue, sliderValue))
        {
            setParameterValue(masterName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getMasterSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Master), value);
        }
    };


    addAndMakeVisible(ampBassKnob);
    ampBassKnob.setLookAndFeel(&blueLookAndFeel);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampBassKnob.setNumDecimalPlacesToDisplay(1);
    ampBassKnob.addListener(this);
    ampBassKnob.setRange(0.0, 1.0);
    ampBassKnob.setValue(0.5);
    ampBassKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampBassKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampBassKnob.setNumDecimalPlacesToDisplay(2);
    ampBassKnob.setDoubleClickReturnValue(true, 0.5);

    auto bassValue = getParameterValue(bassName);
    Slider& bassSlider = getBassSlider();
    bassSlider.setValue(bassValue, NotificationType::dontSendNotification);

    ampBassKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getBassSlider().getValue());
        const float bassValue = getParameterValue(bassName);

        if (!approximatelyEqual(bassValue, sliderValue))
        {
            setParameterValue(bassName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getBassSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Bass), value);
        }
    };

    addAndMakeVisible(ampMidKnob);
    ampMidKnob.setLookAndFeel(&blueLookAndFeel);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMidKnob.setNumDecimalPlacesToDisplay(1);
    ampMidKnob.addListener(this);
    ampMidKnob.setRange(0.0, 1.0);
    ampMidKnob.setValue(0.5);
    ampMidKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampMidKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampMidKnob.setNumDecimalPlacesToDisplay(2);
    ampMidKnob.setDoubleClickReturnValue(true, 0.5);

    auto midValue = getParameterValue(midName);
    Slider& midSlider = getMidSlider();
    midSlider.setValue(midValue, NotificationType::dontSendNotification);

    ampMidKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getMidSlider().getValue());
        const float midValue = getParameterValue(midName);

        if (!approximatelyEqual(midValue, sliderValue))
        {
            setParameterValue(midName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getMidSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Mid), value);
        }
    };

    addAndMakeVisible(ampTrebleKnob);
    ampTrebleKnob.setLookAndFeel(&blueLookAndFeel);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampTrebleKnob.setNumDecimalPlacesToDisplay(1);
    ampTrebleKnob.addListener(this);
    ampTrebleKnob.setRange(0.0, 1.0);
    ampTrebleKnob.setValue(0.5);
    ampTrebleKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampTrebleKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampTrebleKnob.setNumDecimalPlacesToDisplay(2);
    ampTrebleKnob.setDoubleClickReturnValue(true, 0.5);

    auto trebleValue = getParameterValue(trebleName);
    Slider& trebleSlider = getTrebleSlider();
    trebleSlider.setValue(trebleValue, NotificationType::dontSendNotification);

    ampTrebleKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getTrebleSlider().getValue());
        const float trebleValue = getParameterValue(trebleName);

        if (!approximatelyEqual(trebleValue, sliderValue))
        {
            setParameterValue(trebleName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getTrebleSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Treble), value);
        }
    };

    addAndMakeVisible(ampPresenceKnob);
    ampPresenceKnob.setLookAndFeel(&blueLookAndFeel);
    ampPresenceKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampPresenceKnob.setNumDecimalPlacesToDisplay(1);
    ampPresenceKnob.addListener(this);
    ampPresenceKnob.setRange(0.0, 1.0);
    ampPresenceKnob.setValue(0.5);
    ampPresenceKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampPresenceKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampPresenceKnob.setNumDecimalPlacesToDisplay(2);
    ampPresenceKnob.setDoubleClickReturnValue(true, 0.5);

    auto presenceValue = getParameterValue(trebleName);
    Slider& presenceSlider = getPresenceSlider();
    trebleSlider.setValue(presenceValue, NotificationType::dontSendNotification);

    ampPresenceKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getPresenceSlider().getValue());
        const float presenceValue = getParameterValue(presenceName);

        if (!approximatelyEqual(presenceValue, sliderValue))
        {
            setParameterValue(presenceName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getPresenceSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Presence), value);
        }
    };

    addAndMakeVisible(ampDelayKnob);
    ampDelayKnob.setLookAndFeel(&blueLookAndFeel);
    ampDelayKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampDelayKnob.setNumDecimalPlacesToDisplay(1);
    ampDelayKnob.addListener(this);
    ampDelayKnob.setRange(0.0, 1.0);
    ampDelayKnob.setValue(0.0);
    ampDelayKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampDelayKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampDelayKnob.setNumDecimalPlacesToDisplay(2);
    ampDelayKnob.setDoubleClickReturnValue(true, 0.0);

    auto delayValue = getParameterValue(delayName);
    Slider& delaySlider = getDelaySlider();
    delaySlider.setValue(delayValue, NotificationType::dontSendNotification);

    ampDelayKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getDelaySlider().getValue());
        const float delayValue = getParameterValue(delayName);

        if (!approximatelyEqual(delayValue, sliderValue))
        {
            setParameterValue(delayName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getDelaySlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Delay), value);
        }
    };

    addAndMakeVisible(ampReverbKnob);
    ampReverbKnob.setLookAndFeel(&blueLookAndFeel);
    ampReverbKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampReverbKnob.setNumDecimalPlacesToDisplay(1);
    ampReverbKnob.addListener(this);
    ampReverbKnob.setRange(0.0, 1.0);
    ampReverbKnob.setValue(0.0);
    ampReverbKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ampReverbKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    ampReverbKnob.setNumDecimalPlacesToDisplay(2);
    ampReverbKnob.setDoubleClickReturnValue(true, 0.0);

    auto reverbValue = getParameterValue(reverbName);
    Slider& reverbSlider = getReverbSlider();
    reverbSlider.setValue(reverbValue, NotificationType::dontSendNotification);

    ampReverbKnob.onValueChange = [this]
    {
        const float sliderValue = static_cast<float> (getReverbSlider().getValue());
        const float reverbValue = getParameterValue(reverbName);

        if (!approximatelyEqual(reverbValue, sliderValue))
        {
            setParameterValue(reverbName, sliderValue);

            // create and send an OSC message with an address and a float value:
            float value = static_cast<float> (getReverbSlider().getValue());
            m_rcSrv.updateKnob(static_cast<int32_t>(NpRpcProto::ESliderId::Reverb), value);
        }
    };

    addAndMakeVisible(GainLabel);
    GainLabel.setText("Gain", juce::NotificationType::dontSendNotification);
    GainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(LevelLabel);
    LevelLabel.setText("Level", juce::NotificationType::dontSendNotification);
    LevelLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(BassLabel);
    BassLabel.setText("Bass", juce::NotificationType::dontSendNotification);
    BassLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(MidLabel);
    MidLabel.setText("Mid", juce::NotificationType::dontSendNotification);
    MidLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(TrebleLabel);
    TrebleLabel.setText("Treble", juce::NotificationType::dontSendNotification);
    TrebleLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(PresenceLabel);
    PresenceLabel.setText("Presence", juce::NotificationType::dontSendNotification);
    PresenceLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(DelayLabel);
    DelayLabel.setText("Delay", juce::NotificationType::dontSendNotification);
    DelayLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(ReverbLabel);
    ReverbLabel.setText("Reverb", juce::NotificationType::dontSendNotification);
    ReverbLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(toneDropDownLabel);
    toneDropDownLabel.setText("Tone", juce::NotificationType::dontSendNotification);
    toneDropDownLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(irDropDownLabel);
    irDropDownLabel.setText("IR", juce::NotificationType::dontSendNotification);
    irDropDownLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(versionLabel);
    versionLabel.setText("v1.3.0", juce::NotificationType::dontSendNotification);
    versionLabel.setJustificationType(juce::Justification::centred);

    auto font = GainLabel.getFont();
    float height = font.getHeight();
    font.setHeight(height); // 0.75);
    GainLabel.setFont(font);
    LevelLabel.setFont(font);
    BassLabel.setFont(font);
    MidLabel.setFont(font);
    TrebleLabel.setFont(font);
    PresenceLabel.setFont(font);
    DelayLabel.setFont(font);
    ReverbLabel.setFont(font);
    toneDropDownLabel.setFont(font);
    irDropDownLabel.setFont(font);
    versionLabel.setFont(font);


    // Name controls:
    addAndMakeVisible(ampNameLabel);
    ampNameField.setEditable(true, true, true);
    addAndMakeVisible(ampNameField);

    // IP controls:
    ipField.setEditable(false, false, false);
    addAndMakeVisible(ipLabel);
    addAndMakeVisible(ipField);

    // Remote controls:
    addAndMakeVisible(rcConnectedLabel);

    // Size of plugin GUI
    setSize(345, 455);

    // Set gain knob color based on conditioned/snapshot model 
    setParamKnobColor();

    m_rcSrv.startThread();
}

NeuralPiAudioProcessorEditor::~NeuralPiAudioProcessorEditor()
{
}

//==============================================================================
void NeuralPiAudioProcessorEditor::paint (Graphics& g)
{
    // Workaround for graphics on Windows builds (clipping code doesn't work correctly on Windows)
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    g.drawImageAt(background, 0, 0);  // Debug Line: Redraw entire background image
#else
// Redraw only the clipped part of the background image
    juce::Rectangle<int> ClipRect = g.getClipBounds();
    g.drawImage(background, ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight(), ClipRect.getX(), ClipRect.getY(), ClipRect.getWidth(), ClipRect.getHeight());
#endif
   
}

void NeuralPiAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    modelSelect.setBounds(11, 10, 270, 25);
    loadButton.setBounds(11, 74, 100, 25);
    modelKnob.setBounds(140, 40, 75, 95);

    irSelect.setBounds(11, 42, 270, 25);
    loadIR.setBounds(120, 74, 100, 25);
    irButton.setBounds(248, 42, 257, 25);
    lstmButton.setBounds(248, 10, 257, 25);

    // Amp Widgets
    ampGainKnob.setBounds(10, 120, 75, 95);
    ampMasterKnob.setBounds(95, 120, 75, 95);
    ampBassKnob.setBounds(10, 250, 75, 95);
    ampMidKnob.setBounds(95, 250, 75, 95);
    ampTrebleKnob.setBounds(180, 250, 75, 95);
    ampPresenceKnob.setBounds(265, 250, 75, 95);

    ampDelayKnob.setBounds(180, 120, 75, 95);
    ampReverbKnob.setBounds(265, 120, 75, 95);

    GainLabel.setBounds(6, 108, 80, 10);
    LevelLabel.setBounds(93, 108, 80, 10);
    BassLabel.setBounds(6, 238, 80, 10);
    MidLabel.setBounds(91, 238, 80, 10);
    TrebleLabel.setBounds(178, 238, 80, 10);
    PresenceLabel.setBounds(265, 238, 80, 10);
    DelayLabel.setBounds(178, 108, 80, 10);
    ReverbLabel.setBounds(265, 108, 80, 10);

    toneDropDownLabel.setBounds(267, 16, 80, 10);
    irDropDownLabel.setBounds(261, 48, 80, 10);
    versionLabel.setBounds(268, 431, 80, 10);

    addAndMakeVisible(ampNameLabel);
    ampNameField.setEditable(true, true, true);
    addAndMakeVisible(ampNameField);

    // IP controls:
    ipLabel.setBounds(15, 365, 125, 25);
    ipField.setBounds(140, 365, 100, 25);
    rcConnectedLabel.setBounds(240, 365, 80, 25);
}

void NeuralPiAudioProcessorEditor::modelSelectChanged(int index)
{
    if (index >= 0 && index < processor.jsonFiles.size()) {
        File selectedFile = processor.userAppDataDirectory_tones.getFullPathName() + "/" + modelSelect.getText() + ".json";
        //processor.loadConfig(processor.jsonFiles[selectedFileIndex]);
        processor.loadConfig(selectedFile);
        processor.current_model_index = index;
    }
    auto newValue = static_cast<float>(processor.current_model_index / (processor.num_models - 1.0));
    modelKnob.setValue(newValue);
    setParamKnobColor();
}

void NeuralPiAudioProcessorEditor::irSelectChanged(int index)
{
    if (index >= 0 && index < processor.irFiles.size()) {
        File selectedFile = processor.userAppDataDirectory_irs.getFullPathName() + "/" + irSelect.getText() + ".wav";
        //processor.loadIR(processor.irFiles[selectedFileIndex]);
        processor.loadIR(selectedFile);
        processor.current_ir_index = index;
    }
    auto newValue = static_cast<float>(processor.current_ir_index / (processor.num_irs - 1.0));
    irKnob.setValue(newValue);
}

void NeuralPiAudioProcessorEditor::updateToggleState(juce::Button* button, juce::String name)
{
    if (name == "IR")
        processor.ir_state = button->getToggleState();
    else
        processor.lstm_state = button->getToggleState();
}

void NeuralPiAudioProcessorEditor::loadButtonClicked()
{  
    myChooser = std::make_unique<FileChooser> ("Select one or more .json tone files to import",
                                               File::getSpecialLocation (File::userDesktopDirectory),
                                               "*.json");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;

    myChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)
    {
        Array<File> files = chooser.getResults();
        for (auto file : files) {
            File fullpath = processor.userAppDataDirectory_tones.getFullPathName() + "/" + file.getFileName();
            bool b = fullpath.existsAsFile();
            if (b == false) {

                processor.loadConfig(file);
                fname = file.getFileName();
                processor.loaded_tone = file;
                processor.loaded_tone_name = fname;
                processor.custom_tone = 1;

                // Copy selected file to model directory and load into dropdown menu
                bool a = file.copyFileTo(fullpath);
                if (a == true) {
                    modelSelect.addItem(file.getFileNameWithoutExtension(), processor.jsonFiles.size() + 1);
                    modelSelect.setSelectedItemIndex(processor.jsonFiles.size(), juce::NotificationType::dontSendNotification);
                    processor.jsonFiles.push_back(file);
                    processor.num_models += 1;
                }
                // Sort jsonFiles alphabetically
                std::sort(processor.jsonFiles.begin(), processor.jsonFiles.end());
            }
        }
    });
    setParamKnobColor();
}

void NeuralPiAudioProcessorEditor::loadIRClicked()
{
    myChooser = std::make_unique<FileChooser> ("Select one or more .wav IR files to import",
                                               File::getSpecialLocation (File::userDesktopDirectory),
                                               "*.wav");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems;

    myChooser->launchAsync (folderChooserFlags, [this] (const FileChooser& chooser)   
    {
        Array<File> files = chooser.getResults();
        for (auto file : files) {
            File fullpath = processor.userAppDataDirectory_irs.getFullPathName() + "/" + file.getFileName();
            bool b = fullpath.existsAsFile();
            if (b == false) {

                processor.loadIR(file);
                fname = file.getFileName();
                processor.loaded_ir = file;
                processor.loaded_ir_name = fname;
                processor.custom_ir = 1;

                // Copy selected file to model directory and load into dropdown menu
                bool a = file.copyFileTo(fullpath);
                if (a == true) {
                    irSelect.addItem(file.getFileNameWithoutExtension(), processor.irFiles.size() + 1);
                    irSelect.setSelectedItemIndex(processor.irFiles.size(), juce::NotificationType::dontSendNotification);
                    processor.irFiles.push_back(file);
                    processor.num_irs += 1;
                }
                // Sort jsonFiles alphabetically
                std::sort(processor.irFiles.begin(), processor.irFiles.end());
            }
        }
    });
}


void NeuralPiAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton) {
        loadButtonClicked();
    }
    else
    {
        loadIRClicked();
    }
}


void NeuralPiAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &modelKnob) {
        if (slider->getValue() >= 0 && slider->getValue() < processor.jsonFiles.size()) {
            modelSelect.setSelectedItemIndex(processor.getModelIndex(slider->getValue()), juce::NotificationType::dontSendNotification);
        }
    } else if (slider == &irKnob) {
        if (slider->getValue() >= 0 && slider->getValue() < processor.irFiles.size()) {
            irSelect.setSelectedItemIndex(processor.getIrIndex(slider->getValue()), juce::NotificationType::dontSendNotification);
        }
    }
}

// OSC Messages
Slider& NeuralPiAudioProcessorEditor::getGainSlider()
{
    return ampGainKnob;
}

Slider& NeuralPiAudioProcessorEditor::getMasterSlider()
{
    return ampMasterKnob;
}

Slider& NeuralPiAudioProcessorEditor::getBassSlider()
{
    return ampBassKnob;
}

Slider& NeuralPiAudioProcessorEditor::getMidSlider()
{
    return ampMidKnob;
}

Slider& NeuralPiAudioProcessorEditor::getTrebleSlider()
{
    return ampTrebleKnob;
}

Slider& NeuralPiAudioProcessorEditor::getPresenceSlider()
{
    return ampPresenceKnob;
}

Slider& NeuralPiAudioProcessorEditor::getDelaySlider()
{
    return ampDelayKnob;
}

Slider& NeuralPiAudioProcessorEditor::getReverbSlider()
{
    return ampReverbKnob;
}

Slider& NeuralPiAudioProcessorEditor::getModelSlider()
{
    return modelKnob;
}

Slider& NeuralPiAudioProcessorEditor::getIrSlider()
{
    return irKnob;
}

Label& NeuralPiAudioProcessorEditor::getIPField()
{
    return ipField;
}

Label& NeuralPiAudioProcessorEditor::getAmpNameField()
{
    return ampNameField;
}

Label& NeuralPiAudioProcessorEditor::getOutConnectedLabel()
{
    return rcConnectedLabel;
}

void NeuralPiAudioProcessorEditor::updateOutConnectedLabel(bool connected)
{
    if (connected)
    {
        //getOutConnectedLabel().setText("Connected", dontSendNotification);
        rcConnectedLabel.setText("Connected", dontSendNotification);

    }
    else
    {
        getOutConnectedLabel().setText("", dontSendNotification);
        rcConnectedLabel.setText("", dontSendNotification);
        ipField.setText("", dontSendNotification);
    }
}

// This callback is invoked if an OSC message has been received setting either value.
void NeuralPiAudioProcessorEditor::valueChanged(Value& value)
{
//    if (value.refersToSameSourceAs(oscReceiver.getGainValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getGainSlider().getValue()))
//        {
//            getGainSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    else if (value.refersToSameSourceAs(oscReceiver.getMasterValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getMasterSlider().getValue()))
//        {
//            getMasterSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    if (value.refersToSameSourceAs(oscReceiver.getBassValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getBassSlider().getValue()))
//        {
//            getBassSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    else if (value.refersToSameSourceAs(oscReceiver.getMidValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getMidSlider().getValue()))
//        {
//            getMidSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    if (value.refersToSameSourceAs(oscReceiver.getTrebleValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getTrebleSlider().getValue()))
//        {
//            getTrebleSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    else if (value.refersToSameSourceAs(oscReceiver.getPresenceValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getPresenceSlider().getValue()))
//        {
//            getPresenceSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    if (value.refersToSameSourceAs(oscReceiver.getDelayValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getDelaySlider().getValue()))
//        {
//            getDelaySlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    else if (value.refersToSameSourceAs(oscReceiver.getReverbValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getReverbSlider().getValue()))
//        {
//            getReverbSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    else if (value.refersToSameSourceAs(oscReceiver.getModelValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getModelSlider().getValue()))
//        {
//            getModelSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
//    else if (value.refersToSameSourceAs(oscReceiver.getIrValue()))
//    {
//        if (!approximatelyEqual(static_cast<double> (value.getValue()), getIrSlider().getValue()))
//        {
//            getIrSlider().setValue(static_cast<double> (value.getValue()),
//                NotificationType::sendNotification);
//        }
//    }
}

void NeuralPiAudioProcessorEditor::timerCallback()
{
    getGainSlider().setValue(getParameterValue(gainName), NotificationType::dontSendNotification);
    getMasterSlider().setValue(getParameterValue(masterName), NotificationType::dontSendNotification);
    getBassSlider().setValue(getParameterValue(bassName), NotificationType::dontSendNotification);
    getMidSlider().setValue(getParameterValue(midName), NotificationType::dontSendNotification);
    getTrebleSlider().setValue(getParameterValue(trebleName), NotificationType::dontSendNotification);
    getPresenceSlider().setValue(getParameterValue(presenceName), NotificationType::dontSendNotification);
    getDelaySlider().setValue(getParameterValue(delayName), NotificationType::dontSendNotification);
    getReverbSlider().setValue(getParameterValue(reverbName), NotificationType::dontSendNotification);
    getModelSlider().setValue(getParameterValue(modelName), NotificationType::dontSendNotification);
    getIrSlider().setValue(getParameterValue(irName), NotificationType::dontSendNotification);
}

AudioProcessorParameter* NeuralPiAudioProcessorEditor::getParameter(const String& paramId)
{
    if (auto* proc = getAudioProcessor())
    {
        auto& params = proc->getParameters();

        for (auto p : params)
        {
            if (auto* param = dynamic_cast<AudioProcessorParameterWithID*> (p))
            {
                if (param->paramID == paramId)
                    return param;
            }
        }
    }

    return nullptr;
}

float NeuralPiAudioProcessorEditor::getParameterValue(const String& paramId)
{
    if (auto* param = getParameter(paramId))
        return param->getValue();

    return 0.0f;
}

void NeuralPiAudioProcessorEditor::setParameterValue(const String& paramId, float value)
{
    if (auto* param = getParameter(paramId))
        param->setValueNotifyingHost(value);
}


void NeuralPiAudioProcessorEditor::setParamKnobColor()
{
    // If the knob is used for a parameter, change it to red
    if (processor.params == 0) {
        ampGainKnob.setLookAndFeel(&blueLookAndFeel);
        ampMasterKnob.setLookAndFeel(&blueLookAndFeel);
    }
    else if (processor.params == 1) {
        ampGainKnob.setLookAndFeel(&redLookAndFeel);
        ampMasterKnob.setLookAndFeel(&blueLookAndFeel);
    }
    else if (processor.params == 2) {
        ampGainKnob.setLookAndFeel(&redLookAndFeel);
        ampMasterKnob.setLookAndFeel(&redLookAndFeel);
    }

}


void NeuralPiAudioProcessorEditor::updateKnob(int id, float value) {
    /* TBD: change magic numbers:
        case 0: to case NpProto::GainKnobIdId:
        case 1: to case NpProto::MasterKnobId:
        . . .
    */
    switch (id) {
    case static_cast<int>(NpRpcProto::ESliderId::Gain):
        if (!approximatelyEqual(getParameterValue(gainName), value)) {
             setParameterValue(gainName, value);
             getGainSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Master):
        if (!approximatelyEqual(getParameterValue(masterName), value)) {
             setParameterValue(masterName, value);
             getMasterSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Delay):
        if (!approximatelyEqual(getParameterValue(delayName), value)) {
             setParameterValue(delayName, value);
             getDelaySlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Reverb):
        if (!approximatelyEqual(getParameterValue(reverbName), value)) {
             setParameterValue(reverbName, value);
             getReverbSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Bass):
        if (!approximatelyEqual(getParameterValue(bassName), value)) {
             setParameterValue(bassName, value);
             getBassSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Mid):
        if (!approximatelyEqual(getParameterValue(midName), value)) {
             setParameterValue(midName, value);
             getMidSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Treble):
        if (!approximatelyEqual(getParameterValue(trebleName), value)) {
             setParameterValue(trebleName, value);
             getTrebleSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
    case static_cast<int>(NpRpcProto::ESliderId::Presence):
        if (!approximatelyEqual(getParameterValue(presenceName), value)) {
             setParameterValue(presenceName, value);
             getPresenceSlider().setValue(value, NotificationType::dontSendNotification);
         }
         break;
     default: break;
    }
}

void NeuralPiAudioProcessorEditor::updateModelIndex(int id, int index) {
    switch (id) {
    case static_cast<int>(NpRpcProto::EComboBoxId::Model):
        modelSelect.setSelectedItemIndex(index, NotificationType::dontSendNotification); 
        modelSelectChanged(index);
        break;
    case static_cast<int>(NpRpcProto::EComboBoxId::Ir):
        irSelect.setSelectedItemIndex(index, NotificationType::dontSendNotification); 
        irSelectChanged(index);
        break;
    default:
        break;
    }
}

void NeuralPiAudioProcessorEditor::addModelItem(int id, juce::String itemValue, int itemIndex) {
}

void NeuralPiAudioProcessorEditor::onStateChanged(IUdpRcListener::EState prevState, IUdpRcListener::EState state) {
    switch (state) {
    case IUdpRcListener::EState::Connected:
        updateOutConnectedLabel(true);
        break;
    case IUdpRcListener::EState::Idle:
    case IUdpRcListener::EState::ReqScan:
    case IUdpRcListener::EState::Scanning:
    case IUdpRcListener::EState::ReqConnect:
    case IUdpRcListener::EState::Connecting:
    case IUdpRcListener::EState::Disconnecting:
    case IUdpRcListener::EState::Error:
    default:
        updateOutConnectedLabel(false);
        break;
    }
}

void NeuralPiAudioProcessorEditor::onConnReceived(const juce::String addr) {
    {
        ipField.setText(addr, NotificationType::sendNotification);

        for (auto i = 0; i < modelSelect.getNumItems(); i++) {
            m_rcSrv.addModelItem(static_cast<int32_t>(NpRpcProto::EComboBoxId::Model), 
                modelSelect.getItemText(i), modelSelect.getItemId(i));
        }

        for (auto i = 0; i < irSelect.getNumItems(); i++) {
            m_rcSrv.addModelItem(static_cast<int32_t>(NpRpcProto::EComboBoxId::Ir),
                irSelect.getItemText(i), irSelect.getItemId(i));
        }

        m_rcSrv.finishConfig();

        m_rcSrv.updateModelIndex(static_cast<int>(NpRpcProto::EComboBoxId::Model), modelSelect.getSelectedItemIndex());
        m_rcSrv.updateModelIndex(static_cast<int>(NpRpcProto::EComboBoxId::Ir),    irSelect.getSelectedItemIndex());

        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Gain),     ampGainKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Master),   ampMasterKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Delay),    ampDelayKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Reverb),   ampReverbKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Bass),     ampBassKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Mid),      ampMidKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Treble),   ampTrebleKnob.getValue());
        m_rcSrv.updateKnob(static_cast<int>(NpRpcProto::ESliderId::Presence), ampPresenceKnob.getValue());
    }
}
