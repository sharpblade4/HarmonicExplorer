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

    for (int i = 0; i < AMOUNT_OF_HARMONICS; ++i)     {
        knobs[i].setSliderStyle (juce::Slider::RotaryHorizontalDrag);
        knobs[i].setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
        knobs[i].setRange (0.0, 1.0, 0.01); // Set the range according to your needs
        knobs[i].setValue (processorRef.getLevelAt(i)); // Set an initial value
        knobs[i].addListener (this); // Attach a listener to the knob
        knobs->setWantsKeyboardFocus(false);
        addAndMakeVisible (knobs[i]);
    }

    keyboardComponent.setWantsKeyboardFocus(true);
    grabKeyboardFocus();
    setSize (600, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

/* callback for the knobs change */
void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) { 
    int sliderIndex = -1;
    for (int i=0; i<AMOUNT_OF_HARMONICS; ++i) {
        if (slider == &knobs[i]) {
            sliderIndex = i;
            break;
        }
    }

    if (sliderIndex < 0) {
        return;
    }

    processorRef.setLevelAt(sliderIndex, (float) slider->getValue());
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

    for (int i = 0; i < AMOUNT_OF_HARMONICS; ++i) {
        knobs[i].setBounds(50 + i * 120, 150, 100, 100);
    }

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
