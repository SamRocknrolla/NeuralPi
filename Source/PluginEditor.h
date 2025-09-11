/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "UdpRcServer.h"
#include <stdlib.h>

//==============================================================================
/**
*/
class NeuralPiAudioProcessorEditor  : public AudioProcessorEditor
                                    , public IUdpRcServerListener
                                    , private Button::Listener
                                    , private Slider::Listener                               
                                    , private ComboBox::Listener
{
public:
    NeuralPiAudioProcessorEditor (NeuralPiAudioProcessor&);
    ~NeuralPiAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;


    String ampName{ "NeuralPi" };

    const String gainName{ "gain" };
    const String masterName{ "master" };
    const String bassName{ "bass" };
    const String midName{ "mid" };
    const String trebleName{ "treble" };
    const String presenceName{ "presence" };
    const String delayName{ "delay" };
    const String reverbName{ "reverb" };

    const String modelName{ "model" };
    const String irName{ "ir" };

    // For the FileChooser to load json models and IR files
    std::unique_ptr<FileChooser> myChooser;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NeuralPiAudioProcessor& processor;

    std::unique_ptr < UdpRcServer> m_rcSrv;
    //UdpRcServer m_rcSrv;

    Image background = ImageCache::getFromMemory(BinaryData::npi_background_jpg, BinaryData::npi_background_jpgSize);

    // Amp Widgets
    Slider ampGainKnob;
    Slider ampMasterKnob;
    Slider ampBassKnob;
    Slider ampMidKnob;
    Slider ampTrebleKnob;
    Slider ampPresenceKnob;
    Slider ampDelayKnob;
    Slider ampReverbKnob;

    IdPtrMap<NpRpcProto::ESliderId, juce::Slider*, static_cast<int>(NpRpcProto::ESliderId::MAX)> m_sliderMap;

    //====================================================================
    // UI initialization
    //====================================================================
    void initKnobSlider(juce::Slider& slider, String sliderId, float value, SliderListener* listener);
    void setupUI();

    //ImageButton ampOnButton;
    //ImageButton ampLED;
    ComboBox modelSelect;
    TextButton nextModelButton;
    TextButton prevModelButton;

    ComboBox irSelect;
    TextButton nextIrButton;
    TextButton prevIrButton;

    Label GainLabel;
    Label LevelLabel;
    Label BassLabel;
    Label MidLabel;
    Label TrebleLabel;
    Label PresenceLabel;
    Label DelayLabel;
    Label ReverbLabel;
    Label toneDropDownLabel;
    Label irDropDownLabel;
    Label versionLabel;

    File test_file;
    File model_folder;

    TextButton loadButton;
    TextButton loadIR;
    ToggleButton irButton;
    ToggleButton lstmButton;

    juce::String fname;

    void modelSelectChanged(int index);
    void loadButtonClicked();
    void setNextComboBoxItem(ComboBox& cbox);
    void setPrevComboBoxItem(ComboBox& cbox);
    void updateToggleState(juce::Button* button, juce::String name);
    void irSelectChanged(int Index);
    void loadIRClicked();

    virtual void buttonClicked(Button* button) override;
    virtual void sliderValueChanged(Slider* slider) override;
    virtual void comboBoxChanged(ComboBox* cbox) override;


    Label ampNameLabel{ {}, "Amp Name (no spaces): " };
    Label ampNameField{ {}, "NeuralPi" };

    Label ipLabel{ {}, "Remote Control IPv4: " };
    Label ipField{ {}, "N/A" };
    juce::String rcIpAddr;

    Label gainLabel{ {}, "Gain" };
    Label masterLabel{ {}, "Master" };

    Label modelLabel{ {}, "Model" };

    Label rcConnectedLabel{ "(connected)" };

    void updateOutConnectedLabel(bool connected);

    AudioProcessorParameter* getParameter(const String& paramId);

    float getParameterValue(const String& paramId);
    void setParameterValue(const String& paramId, float value);
    void setParamKnobColor();

    virtual void updateKnob(int id, float value);
    virtual void updateModelIndex(int id, int index);
    virtual void addModelItem(int id, juce::String itemValue, int itemIndex);

    virtual void onStateChanged(IUdpRcListener::EState prevState, IUdpRcListener::EState state);

    virtual void onConnReceived(const juce::String addr);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeuralPiAudioProcessorEditor)
};
