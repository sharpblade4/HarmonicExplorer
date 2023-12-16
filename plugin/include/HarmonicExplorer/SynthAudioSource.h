#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_basics/juce_audio_basics.h>

/*  A SynthesiserSound is a type of sound but in practice a control over what notes/channels can be played. 
    The Synthesiser assigns a SynthesiserSound to a SynthesiserVoice which plays under that SynthesiserSound.
    Effectively, the Synthesiser plays SynthesiserVoice when notes are played.  
*/

static const int AMOUNT_OF_HARMONICS = 4;


////////////////// SINE WAVE SOUND  ////////////////////////
struct SineWaveSound   : public juce::SynthesiserSound
{
    SineWaveSound() {}
 
    bool appliesToNote    (int) override        { return true; }
    bool appliesToChannel (int) override        { return true; }
};


////////////////// SINE WAVE VOICE  ////////////////////////
struct SineWaveVoice  : public juce::SynthesiserVoice
{
    SineWaveVoice(float* levels);

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SineWaveSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override;

    void stopNote (float /*velocity*/, bool allowTailOff) override;
    
    void pitchWheelMoved (int /*newValue*/) override                              {}
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override    {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    using SynthesiserVoice::renderNextBlock;

private:
    double tailOff = 0.0;
    double currentAngles[AMOUNT_OF_HARMONICS] = {0.0};
    double anglesDeltas[AMOUNT_OF_HARMONICS] = {0.0};
    float harmonicFactors[AMOUNT_OF_HARMONICS] = {0.0};
    float* levels;

    void computeSample(juce::AudioBuffer<float>& outputBuffer, int startSample);
};


////////////////// SYNTH AUDIO SOURCE  ////////////////////////
class SynthAudioSource   : public juce::AudioSource
{
public:
    SynthAudioSource (juce::MidiKeyboardState& keyState);
 
    void setUsingSineWaveSound();
 
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override;
 
    void releaseResources() override {}
 
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    void setLevelAt(int index, float value) {
        if (index < AMOUNT_OF_HARMONICS && index >= 0 && value <= 1.0 && value >= 0) {
            levelsArr[index] = value;
        }
    }
    float getLevelAt(int index) {
        return index < AMOUNT_OF_HARMONICS && index >= 0 ? levelsArr[index] : 0.0; 
    }
 
private:
    juce::MidiKeyboardState& keyboardState;
    juce::Synthesiser synth;

    float levelsArr[AMOUNT_OF_HARMONICS] = {0.0};
};