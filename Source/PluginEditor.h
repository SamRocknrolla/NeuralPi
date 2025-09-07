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
class NeuralPiAudioProcessorEditor  : public AudioProcessorEditor,
                                      private Button::Listener,
                                      private Slider::Listener,
                                      private Value::Listener,
                                      private Timer,
                                      private IUdpRcServerListener
                                
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

    //std::unique_ptr < UdpRcServer> m_rcSrv;
    UdpRcServer m_rcSrv;

    Image background = ImageCache::getFromMemory(BinaryData::npi_background_jpg, BinaryData::npi_background_jpgSize);

    // Amp Widgets
    Slider ampGainKnob;
    Slider ampMasterKnob;
    Slider modelKnob;
    Slider irKnob;
    //ImageButton ampOnButton;
    //ImageButton ampLED;
    ComboBox modelSelect;
    ComboBox irSelect;
    Slider ampBassKnob;
    Slider ampMidKnob;
    Slider ampTrebleKnob;
    Slider ampPresenceKnob;
    Slider ampDelayKnob;
    Slider ampReverbKnob;

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

    juce::LookAndFeel_V4 blueLookAndFeel;
    juce::LookAndFeel_V4 redLookAndFeel;
    
    juce::String fname;
    virtual void buttonClicked(Button* button) override;
    void modelSelectChanged(int index);
    void loadButtonClicked();
    void updateToggleState(juce::Button* button, juce::String name);
    void irSelectChanged(int Index);
    void loadIRClicked();
    virtual void sliderValueChanged(Slider* slider) override;


    Label ampNameLabel{ {}, "Amp Name (no spaces): " };
    Label ampNameField{ {}, "NeuralPi" };

    Label ipLabel{ {}, "Remote Control IP: " };
    Label ipField{ {}, "" };

    Label gainLabel{ {}, "Gain" };
    Label masterLabel{ {}, "Master" };

    Label modelLabel{ {}, "Model" };

    Label rcConnectedLabel{ "(connected)" };

    // OSC Messages
    Slider& getGainSlider();
    Slider& getMasterSlider();
    Slider& getModelSlider();
    Slider& getIrSlider();
    Slider& getBassSlider();
    Slider& getMidSlider();
    Slider& getTrebleSlider();
    Slider& getPresenceSlider();
    Slider& getDelaySlider();
    Slider& getReverbSlider();

    Label& getIPField();
    Label& getAmpNameField();
    Label& getOutConnectedLabel();

    void updateOutConnectedLabel(bool connected);
    // This callback is invoked if an OSC message has been received setting either value.
    void valueChanged(Value& value) override;
    void timerCallback() override;

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
