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
{
    addAndMakeVisible(modelSelect);
    modelSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int c = 1;
    for (const auto& jsonFile : processor.jsonFiles) {
        modelSelect.addItem(jsonFile.getFileNameWithoutExtension(), c);
        c += 1;
    }
    modelSelect.addListener(this);
    modelSelect.setSelectedItemIndex(processor.current_model_index, juce::NotificationType::dontSendNotification);
    modelSelect.setScrollWheelEnabled(true);
    modelSelect.setComponentID(modelName);

    addAndMakeVisible(nextModelButton);
    nextModelButton.setButtonText(">");
    nextModelButton.setColour(juce::Label::textColourId, juce::Colours::black);
    nextModelButton.addListener(this);
    nextModelButton.setEnabled(true);

    addAndMakeVisible(prevModelButton);
    prevModelButton.setButtonText("<");
    prevModelButton.setColour(juce::Label::textColourId, juce::Colours::black);
    prevModelButton.addListener(this);
    prevModelButton.setEnabled(true);

    addAndMakeVisible(loadButton);
    loadButton.setButtonText("Import Tome");
    loadButton.setColour(juce::Label::textColourId, juce::Colours::black);
    loadButton.addListener(this);

    addAndMakeVisible(irSelect);
    irSelect.setColour(juce::Label::textColourId, juce::Colours::black);
    int i = 1;
    for (const auto& jsonFile : processor.irFiles) {
        irSelect.addItem(jsonFile.getFileNameWithoutExtension(), i);
        i += 1;
    }
    irSelect.addListener(this);
    irSelect.setSelectedItemIndex(processor.current_ir_index, juce::NotificationType::dontSendNotification);
    irSelect.setScrollWheelEnabled(true);
    irSelect.setComponentID(irName);

    addAndMakeVisible(nextIrButton);
    nextIrButton.setColour(juce::Label::textColourId, juce::Colours::black);
    nextIrButton.addListener(this);
    nextIrButton.setButtonText(">");
    nextIrButton.setEnabled(true);

    addAndMakeVisible(prevIrButton);
    prevIrButton.setButtonText("<");
    prevIrButton.setColour(juce::Label::textColourId, juce::Colours::black);
    prevIrButton.addListener(this);
    prevIrButton.setEnabled(true);

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
  
    initKnobSlider(ampGainKnob, gainName, getParameterValue(gainName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Gain, &ampGainKnob);

    initKnobSlider(ampMasterKnob, masterName, getParameterValue(masterName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Master, &ampMasterKnob);

    initKnobSlider(ampBassKnob, bassName, getParameterValue(bassName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Bass, &ampBassKnob);

    initKnobSlider(ampMidKnob, midName, getParameterValue(midName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Mid, &ampMidKnob);

    initKnobSlider(ampTrebleKnob, trebleName, getParameterValue(trebleName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Treble, &ampTrebleKnob);

    initKnobSlider(ampPresenceKnob, presenceName, getParameterValue(presenceName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Presence, &ampPresenceKnob);

    initKnobSlider(ampDelayKnob, delayName, getParameterValue(delayName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Delay, &ampDelayKnob);

    initKnobSlider(ampReverbKnob, reverbName, getParameterValue(reverbName), this);
    m_sliderMap.assign(NpRpcProto::ESliderId::Reverb, &ampReverbKnob);


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

    // Size of plugin GUI
    setSize(345, 455);

    // Set gain knob color based on conditioned/snapshot model 
    setParamKnobColor();

    m_rcSrv = std::make_unique<UdpRcServer>(NpRpcProto::NPRPC_SRV_PORT, NpRpcProto::NPRPC_MCAST_ADDR, *this);
    m_rcSrv->startThread();
}

NeuralPiAudioProcessorEditor::~NeuralPiAudioProcessorEditor()
{
    m_rcSrv->signalThreadShouldExit();
    m_rcSrv->stopThread(1000);
    m_rcSrv.reset(nullptr);
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

void NeuralPiAudioProcessorEditor::resized() {
    setupUI();
}

void NeuralPiAudioProcessorEditor::setupUI()
{
    auto area = getLocalBounds().reduced(10); // Add some margin
    const float dpi = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->dpi;
    const float scale = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale;
    const float mm = dpi / 25.4f / scale;
    const int rowH = mm * 8.0f;
    const int butW = mm * 8.0f;
    const int lblH = mm * 4.0f;
    const int spacing = mm / 2;

    FlexItem::Margin lblMargin{ mm * 2, 0.0, 0.0, 0.0 };
    FlexItem::Margin knobMargin{ 0.0, 0.0, mm * 2, 0.0 };
    const int knobH = area.getWidth() / 4;
    // 1 cm in pixels

    // === ROW 1: Label (Tone) ===
    FlexBox labelRow1;
    labelRow1.flexDirection = FlexBox::Direction::row;
    labelRow1.justifyContent = FlexBox::JustifyContent::spaceBetween;
    labelRow1.items.add(FlexItem(toneDropDownLabel).withFlex(1));

    // === ROW 2: Model Select + Load Button ===
    FlexBox row1;
    row1.flexDirection = FlexBox::Direction::row;
    row1.justifyContent = FlexBox::JustifyContent::center;
    row1.items.add(FlexItem(modelSelect).withFlex(4).withMargin(spacing));
    row1.items.add(FlexItem(prevModelButton).withWidth(butW).withMargin(spacing));
    row1.items.add(FlexItem(nextModelButton).withWidth(butW).withMargin(spacing));

    // === ROW 3: Label (IR) ===
    FlexBox labelRow2;
    labelRow2.flexDirection = FlexBox::Direction::row;
    labelRow2.justifyContent = FlexBox::JustifyContent::spaceBetween;
    labelRow2.items.add(FlexItem(irDropDownLabel).withFlex(1));

    // === ROW 4: IR Select + Load IR + IR Button ===
    FlexBox row2;
    row2.flexDirection = FlexBox::Direction::row;
    row2.justifyContent = FlexBox::JustifyContent::center;
    row2.items.add(FlexItem(irSelect).withFlex(4).withMargin(spacing));
    row2.items.add(FlexItem(prevIrButton).withWidth(butW).withMargin(spacing));
    row2.items.add(FlexItem(nextIrButton).withWidth(butW).withMargin(spacing));

    // === ROW 5: Top Knob Labels (Gain, Master, Delay, Reverb) ===
    FlexBox labelRow3;
    labelRow3.flexDirection = FlexBox::Direction::row;
    labelRow3.justifyContent = FlexBox::JustifyContent::spaceBetween;
    labelRow3.items.add(FlexItem(GainLabel).withFlex(1));
    labelRow3.items.add(FlexItem(LevelLabel).withFlex(1));
    labelRow3.items.add(FlexItem(DelayLabel).withFlex(1));
    labelRow3.items.add(FlexItem(ReverbLabel).withFlex(1));

    // === ROW 6: Top Knob Row (Gain, Master, Delay, Reverb) ===
    FlexBox knobRow1;
    knobRow1.flexDirection = FlexBox::Direction::row;
    knobRow1.justifyContent = FlexBox::JustifyContent::spaceBetween;
    knobRow1.items.add(FlexItem(ampGainKnob).withFlex(1));
    knobRow1.items.add(FlexItem(ampMasterKnob).withFlex(1));
    knobRow1.items.add(FlexItem(ampDelayKnob).withFlex(1));
    knobRow1.items.add(FlexItem(ampReverbKnob).withFlex(1));

    // === ROW 7: Bottom Knob Labels (Bass, Mid, Treble, Presence) ===
    FlexBox labelRow4;
    labelRow4.flexDirection = FlexBox::Direction::row;
    labelRow4.justifyContent = FlexBox::JustifyContent::spaceBetween;
    labelRow4.items.add(FlexItem(BassLabel).withFlex(1));
    labelRow4.items.add(FlexItem(MidLabel).withFlex(1));
    labelRow4.items.add(FlexItem(TrebleLabel).withFlex(1));
    labelRow4.items.add(FlexItem(PresenceLabel).withFlex(1));

    // === ROW 8: Bottom Knob Row (Bass, Mid, Treble, Presence) ===
    FlexBox knobRow2;
    knobRow2.flexDirection = FlexBox::Direction::row;
    knobRow2.justifyContent = FlexBox::JustifyContent::spaceBetween;
    knobRow2.items.add(FlexItem(ampBassKnob).withFlex(1));
    knobRow2.items.add(FlexItem(ampMidKnob).withFlex(1));
    knobRow2.items.add(FlexItem(ampTrebleKnob).withFlex(1));
    knobRow2.items.add(FlexItem(ampPresenceKnob).withFlex(1));

    // === ROW 9: Connection Area ===
    FlexBox connectionRow;
    connectionRow.flexDirection = FlexBox::Direction::row;
    connectionRow.items.add(FlexItem(ipLabel).withFlex(1).withMargin(spacing));
    connectionRow.items.add(FlexItem(ipField).withFlex(1).withMargin(spacing));

    // === ROW 10: Connection Area ===
    FlexBox importRow;
    importRow.flexDirection = FlexBox::Direction::row;
    importRow.items.add(FlexItem(loadButton).withFlex(1));
    importRow.items.add(FlexItem(loadIR).withFlex(1));

    // 
    // === OUTER FlexBox ===
    FlexBox mainFlex;
    mainFlex.flexDirection = FlexBox::Direction::column;

    mainFlex.items.add(FlexItem().withHeight(lblH).withFlex(0).withMargin(spacing));
    mainFlex.items.add(FlexItem(labelRow1).withHeight(lblH).withMargin(lblMargin));
    mainFlex.items.add(FlexItem(row1).withHeight(rowH));
    mainFlex.items.add(FlexItem(labelRow2).withHeight(lblH).withMargin(lblMargin));
    mainFlex.items.add(FlexItem(row2).withHeight(rowH));
    mainFlex.items.add(FlexItem(labelRow3).withHeight(lblH).withMargin(lblMargin));
    mainFlex.items.add(FlexItem(knobRow1).withHeight(knobH).withMargin(knobMargin));
    mainFlex.items.add(FlexItem(labelRow4).withHeight(lblH).withMargin(lblMargin));
    mainFlex.items.add(FlexItem(knobRow2).withHeight(knobH).withMargin(knobMargin));
    mainFlex.items.add(FlexItem(importRow).withHeight(rowH).withMargin(spacing));
    mainFlex.items.add(FlexItem(connectionRow).withHeight(rowH).withMargin(spacing));

    mainFlex.performLayout(area.toFloat());
}

void NeuralPiAudioProcessorEditor::modelSelectChanged(int index)
{
    if (index >= 0 && index < processor.jsonFiles.size()) {
        File selectedFile = processor.userAppDataDirectory_tones.getFullPathName() + "/" + modelSelect.getText() + ".json";
        //processor.loadConfig(processor.jsonFiles[selectedFileIndex]);
        processor.loadConfig(selectedFile);
        processor.current_model_index = index;
    
        float newValue = static_cast<float>(processor.current_model_index / (processor.num_models - 1.0));
        setParameterValue(modelName, newValue);
    }
}

void NeuralPiAudioProcessorEditor::irSelectChanged(int index)
{
    if (index >= 0 && index < processor.irFiles.size()) {
        File selectedFile = processor.userAppDataDirectory_irs.getFullPathName() + "/" + irSelect.getText() + ".wav";
        //processor.loadIR(processor.irFiles[selectedFileIndex]);
        processor.loadIR(selectedFile);
        processor.current_ir_index = index;

        float newValue = static_cast<float>(processor.current_ir_index / (processor.num_irs - 1.0));
        setParameterValue(irName, newValue);
    }
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


void NeuralPiAudioProcessorEditor::initKnobSlider(juce::Slider& slider, juce::String sliderId, float value, SliderListener* listener)
{
    slider.setComponentID(sliderId);

    addAndMakeVisible(slider);
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::aqua);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    slider.setNumDecimalPlacesToDisplay(1);
    slider.addListener(this);
    slider.setRange(0.0, 1.0);
    slider.setValue(0.0);
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    slider.setDoubleClickReturnValue(true, 0.0);

    if (listener != nullptr)
        slider.addListener(listener);

    slider.setValue(value, NotificationType::dontSendNotification);
}

void NeuralPiAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &loadButton) {
        loadButtonClicked();
    }
    else if (button == &loadIR) {
        loadIRClicked();
    }
    else if (button == &nextModelButton) {
        setNextComboBoxItem(modelSelect);
    }
    else if (button == &prevModelButton) {
        setPrevComboBoxItem(modelSelect);
    }
    else if (button == &nextIrButton) {
        setNextComboBoxItem(irSelect);
    }
    else if (button == &prevIrButton) {
        setPrevComboBoxItem(irSelect);
    }
}

void NeuralPiAudioProcessorEditor::setNextComboBoxItem(ComboBox& cbox) {
    if (cbox.getNumItems() > 1) {
        int index = cbox.getSelectedItemIndex() + 1;
        index = (index >= cbox.getNumItems()) ? 0 : index;
        cbox.setSelectedItemIndex(index);
    }
}
void NeuralPiAudioProcessorEditor::setPrevComboBoxItem(ComboBox& cbox) {
    if (cbox.getNumItems() > 1) {
        int index = cbox.getSelectedItemIndex() - 1;
        index = (index < 0) ? cbox.getNumItems() - 1 : index;
        cbox.setSelectedItemIndex(index);
    }
}



void NeuralPiAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    const String sliderId = slider->getComponentID();
    if (sliderId.isNotEmpty()) {
        if (auto* param = getParameter(sliderId)) {
            const float sliderValue = static_cast<float> (slider->getValue());
            const float paramValue = param->getValue();

            if (!approximatelyEqual(paramValue, sliderValue))
            {
                setParameterValue(sliderId, sliderValue);

                // create and send an OSC message with an address and a float value:
                auto id = m_sliderMap.getId(slider);
                if (id != std::nullopt) {
                    m_rcSrv->updateKnob(static_cast<int32_t>(id.value()), static_cast<float>(slider->getValue()));
                }
            }
        }
    }
}

void NeuralPiAudioProcessorEditor::comboBoxChanged(ComboBox* cbox) {
    if (cbox == &modelSelect) {
        int index = modelSelect.getSelectedItemIndex();
        modelSelectChanged(index);
        if (m_rcSrv != nullptr) {
            m_rcSrv->updateModelIndex(static_cast<int32_t>(NpRpcProto::EComboBoxId::Model), index);
        }
        setParamKnobColor();
    }
    else if (cbox == &irSelect) {
        int index = irSelect.getSelectedItemIndex();
        irSelectChanged(index);
        if (m_rcSrv != nullptr) {
            m_rcSrv->updateModelIndex(static_cast<int32_t>(NpRpcProto::EComboBoxId::Ir), index);
        }
    }
}

void NeuralPiAudioProcessorEditor::updateOutConnectedLabel(bool connected)
{
    ipField.setText(((connected)? rcIpAddr : "N/A"),
        dontSendNotification);
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
    auto params = processor.params;
    juce::Colour gainKnobColor = (processor.params > 0)? juce::Colours::red : juce::Colours::aqua;
    juce::Colour masterKnobColor = (processor.params > 1) ? juce::Colours::red : juce::Colours::aqua;

    ampGainKnob.setColour(juce::Slider::thumbColourId, gainKnobColor);
    ampMasterKnob.setColour(juce::Slider::thumbColourId, masterKnobColor);

    if (m_rcSrv != nullptr) {
        m_rcSrv->updateKnobColor(static_cast<int>(NpRpcProto::ESliderId::Gain), gainKnobColor.getARGB());
        m_rcSrv->updateKnobColor(static_cast<int>(NpRpcProto::ESliderId::Master), masterKnobColor.getARGB());
    }
}

void NeuralPiAudioProcessorEditor::updateKnob(int id, float value) {
    Slider* slider = m_sliderMap.getPtr(static_cast<NpRpcProto::ESliderId>(id));
    if (slider != nullptr) {
        String sliderId = slider->getComponentID();
        if (!approximatelyEqual(getParameterValue(sliderId), value)) {
            setParameterValue(sliderId, value);
            slider->setValue(value, NotificationType::dontSendNotification);
        }
    }
}

void NeuralPiAudioProcessorEditor::updateModelIndex(int id, int index) {
    switch (id) {
    case static_cast<int>(NpRpcProto::EComboBoxId::Model):
        modelSelect.setSelectedItemIndex(index, NotificationType::dontSendNotification);
        modelSelectChanged(index);
        setParamKnobColor();

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
        rcIpAddr = addr;

        for (auto i = 0; i < modelSelect.getNumItems(); i++) {
            m_rcSrv->addModelItem(static_cast<int32_t>(NpRpcProto::EComboBoxId::Model), 
                modelSelect.getItemText(i), modelSelect.getItemId(i));
        }

        for (auto i = 0; i < irSelect.getNumItems(); i++) {
            m_rcSrv->addModelItem(static_cast<int32_t>(NpRpcProto::EComboBoxId::Ir),
                irSelect.getItemText(i), irSelect.getItemId(i));
        }

        m_rcSrv->finishConfig();

        m_rcSrv->updateModelIndex(static_cast<int>(NpRpcProto::EComboBoxId::Model), modelSelect.getSelectedItemIndex());
        m_rcSrv->updateModelIndex(static_cast<int>(NpRpcProto::EComboBoxId::Ir),    irSelect.getSelectedItemIndex());

        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Gain),     ampGainKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Master),   ampMasterKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Delay),    ampDelayKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Reverb),   ampReverbKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Bass),     ampBassKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Mid),      ampMidKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Treble),   ampTrebleKnob.getValue());
        m_rcSrv->updateKnob(static_cast<int>(NpRpcProto::ESliderId::Presence), ampPresenceKnob.getValue());

        m_rcSrv->updateKnobColor(static_cast<int>(NpRpcProto::ESliderId::Gain), ampGainKnob.findColour(juce::Slider::thumbColourId).getARGB());
        m_rcSrv->updateKnobColor(static_cast<int>(NpRpcProto::ESliderId::Master), ampMasterKnob.findColour(juce::Slider::thumbColourId).getARGB());
    }
}
