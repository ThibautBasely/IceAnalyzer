#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <algorithm>

IceAudioProcessor::IceAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      bufL ((size_t) fifoSize), bufR ((size_t) fifoSize)
{
}

bool IceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == out;
}

void IceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    // The audio passes through untouched: we only copy it for the analyser.
    const int n = buffer.getNumSamples();
    if (n <= 0 || buffer.getNumChannels() == 0)
        return;

    const float* l = buffer.getReadPointer (0);
    const float* r = buffer.getNumChannels() > 1 ? buffer.getReadPointer (1) : l;

    const int toWrite = std::min (n, fifo.getFreeSpace());
    if (toWrite <= 0)
        return;

    int s1, z1, s2, z2;
    fifo.prepareToWrite (toWrite, s1, z1, s2, z2);
    if (z1 > 0) { std::copy (l, l + z1, bufL.data() + s1); std::copy (r, r + z1, bufR.data() + s1); }
    if (z2 > 0) { std::copy (l + z1, l + z1 + z2, bufL.data() + s2); std::copy (r + z1, r + z1 + z2, bufR.data() + s2); }
    fifo.finishedWrite (z1 + z2);
}

int IceAudioProcessor::pull (float* left, float* right, int maxN)
{
    const int ready = fifo.getNumReady();
    if (ready > maxN)
        fifo.finishedRead (ready - maxN); // drop stale audio (editor was closed)

    const int n = std::min (maxN, fifo.getNumReady());
    if (n <= 0)
        return 0;

    int s1, z1, s2, z2;
    fifo.prepareToRead (n, s1, z1, s2, z2);
    if (z1 > 0) { std::copy (bufL.data() + s1, bufL.data() + s1 + z1, left);      std::copy (bufR.data() + s1, bufR.data() + s1 + z1, right); }
    if (z2 > 0) { std::copy (bufL.data() + s2, bufL.data() + s2 + z2, left + z1); std::copy (bufR.data() + s2, bufR.data() + s2 + z2, right + z1); }
    fifo.finishedRead (z1 + z2);
    return z1 + z2;
}

juce::AudioProcessorEditor* IceAudioProcessor::createEditor()
{
    return new IceAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IceAudioProcessor();
}
