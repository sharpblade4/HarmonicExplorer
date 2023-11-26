#include "HarmonicExplorer/SynthAudioSource.h"

////////////////// SINE WAVE VOICE  ////////////////////////
SineWaveVoice::SineWaveVoice() {
    static_assert(AMOUNT_OF_HARMONICS >= 1, "must have at least 1 harmonic: the fundamental"); 
    for (auto i=0; i<AMOUNT_OF_HARMONICS; ++i) {
        currentAngles[i] = 0.0;
        anglesDeltas[i] = 0.0;
        levels[i] = (float) 0.15 * (AMOUNT_OF_HARMONICS - i)/AMOUNT_OF_HARMONICS;
        harmonicFactors[i] = i == 0 ? 0.25 : i + 4;
    }
}


void SineWaveVoice::startNote(int midiNoteNumber, float velocity,
                juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) 
{
    // level = velocity * 0.15;  TODO use velocity
    juce::ignoreUnused(velocity);

    tailOff = 0.0;
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    auto const sampleRate = getSampleRate();
    for (auto i=0; i<AMOUNT_OF_HARMONICS; ++i) {
        currentAngles[i] = 0.0;
        anglesDeltas[i] = ((cyclesPerSecond * harmonicFactors[i]) / sampleRate) * juce::MathConstants<double>::twoPi;
    }
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
        std::fill(std::begin(anglesDeltas), std::end(anglesDeltas), 0.0);
    }
}

void SineWaveVoice::computeSample(juce::AudioBuffer<float>& outputBuffer, int startSample) {
    float tailOffFactor = tailOff > 0.0 ? (float) tailOff : 1.0;
    float weightedSum = 0.0;
    for (auto i=0; i<AMOUNT_OF_HARMONICS; ++i) {
        weightedSum += ((float) std::sin(currentAngles[i])) * levels[i] * tailOffFactor;
    }
    auto currentSample = weightedSum / AMOUNT_OF_HARMONICS;

    for (auto i = outputBuffer.getNumChannels(); --i >= 0;) {
        outputBuffer.addSample (i, startSample, currentSample);
    }
    for (auto i=0; i<AMOUNT_OF_HARMONICS; ++i) {
            currentAngles[i] += anglesDeltas[i];
    }
}




void SineWaveVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) 
{
    if (! juce::approximatelyEqual (anglesDeltas[0], 0.0)) {
        if (tailOff > 0.0) {
            while (--numSamples >= 0)  {
                computeSample(outputBuffer, startSample);
                ++startSample;

                tailOff *= 0.99;
                if (tailOff <= 0.005)  {
                    stopNote(0.0, false);
                    break;
                }
            }
        } else {
            while (--numSamples >= 0)  {
                computeSample(outputBuffer, startSample);
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