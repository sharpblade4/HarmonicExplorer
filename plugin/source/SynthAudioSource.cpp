#include "HarmonicExplorer/SynthAudioSource.h"

////////////////// SINE WAVE VOICE  ////////////////////////

void SineWaveVoice::startNote(int midiNoteNumber, float velocity,
                juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) 
{
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
}

void SineWaveVoice::stopNote(float /*velocity*/, bool allowTailOff) 
{
    if (allowTailOff)
    {
        // start a tail-off by setting this flag. The render callback will pick up on
        // this and do a fade out, calling clearCurrentNote() when it's finished.

        if (juce::approximatelyEqual (tailOff, 0.0)) // we only need to begin a tail-off if it's not already doing so - the
            tailOff = 1.0;                     // stopNote method could be called more than once.
    }
    else
    {
        // we're being told to stop playing immediately, so reset everything..
        clearCurrentNote();
        angleDelta = 0.0;
    }
}


void SineWaveVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) 
{
    if (! juce::approximatelyEqual (angleDelta, 0.0))
    {
        if (tailOff > 0.0)
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float) (std::sin (currentAngle) * level * tailOff);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;

                tailOff *= 0.99;

                if (tailOff <= 0.005)
                {
                    clearCurrentNote();

                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                auto currentSample = (float) (std::sin (currentAngle) * level);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample (i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
}


////////////////// SYNTH AUDIO SOURCE  ////////////////////////

SynthAudioSource::SynthAudioSource (juce::MidiKeyboardState& keyState)
    : keyboardState (keyState)
{
    for (auto i = 0; i < 5; ++i)                // [1]
        synth.addVoice (new SineWaveVoice());

    synth.addSound (new SineWaveSound());       // [2]
}

void SynthAudioSource::setUsingSineWaveSound()
{
    synth.clearSounds();
}

void SynthAudioSource::prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate (sampleRate); // [3]
}

void SynthAudioSource::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    keyboardState.processNextMidiBuffer (incomingMidi, bufferToFill.startSample,
                                            bufferToFill.numSamples, true);       // [4]

    synth.renderNextBlock (*bufferToFill.buffer, incomingMidi,
                            bufferToFill.startSample, bufferToFill.numSamples); // [5]
}