#include "HarmonicExplorer/PluginProcessor.h"
#include "HarmonicExplorer/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), 
    processorRef(p),
    keyboardComponent(p.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    juce::ignoreUnused(processorRef);
    
    addAndMakeVisible(keyboardComponent);
    // setAudioChannels(0, 2);

    setSize (600, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("Ron U JUCE Plugin!", getLocalBounds(), juce::Justification::centred, 1);

}

void AudioPluginAudioProcessorEditor::resized()
{
    keyboardComponent.setBounds(10, 10, getWidth() - 20, 100);

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
