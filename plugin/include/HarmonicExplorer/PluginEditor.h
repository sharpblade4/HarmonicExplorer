#pragma once

#include "PluginProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>


//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    // void mouseDown(const juce::MouseEvent& event) override {} ;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::MidiKeyboardComponent keyboardComponent;

    juce::Slider knobs[AMOUNT_OF_HARMONICS];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
