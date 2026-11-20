#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <vector>

class IceAudioProcessor : public juce::AudioProcessor
{
public:
    IceAudioProcessor();

    void prepareToPlay (double sr, int) override { sampleRate.store (sr); }
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Ice Analyzer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    // Called from the editor (message thread): reads the newest samples (max maxN)
    int pull (float* left, float* right, int maxN);

    std::atomic<double> sampleRate { 48000.0 };

private:
    static constexpr int fifoSize = 1 << 17;
    juce::AbstractFifo fifo { fifoSize };
    std::vector<float> bufL, bufR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IceAudioProcessor)
};
