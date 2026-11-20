#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include <optional>
#include <vector>

class IceAudioProcessorEditor : public juce::AudioProcessorEditor,
                                private juce::Timer
{
public:
    explicit IceAudioProcessorEditor (IceAudioProcessor&);
    ~IceAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    static std::optional<juce::WebBrowserComponent::Resource> getResource (const juce::String& url);

private:
    void timerCallback() override;

    IceAudioProcessor& processor;
    juce::WebBrowserComponent webView;
    std::vector<float> tmpL, tmpR, interleaved;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IceAudioProcessorEditor)
};
