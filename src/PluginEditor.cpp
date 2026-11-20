#include "PluginEditor.h"
#include "BinaryData.h"
#include <cstring>

static juce::WebBrowserComponent::Options makeOptions()
{
    return juce::WebBrowserComponent::Options{}
        .withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options (juce::WebBrowserComponent::Options::WinWebView2{}
                                    .withUserDataFolder (juce::File::getSpecialLocation (juce::File::tempDirectory)
                                                             .getChildFile ("IceAnalyzerWebView")))
        .withNativeIntegrationEnabled()
        .withResourceProvider ([] (const juce::String& url) { return IceAudioProcessorEditor::getResource (url); });
}

IceAudioProcessorEditor::IceAudioProcessorEditor (IceAudioProcessor& p)
    : AudioProcessorEditor (p), processor (p), webView (makeOptions())
{
    tmpL.resize (32768);
    tmpR.resize (32768);

    addAndMakeVisible (webView);
    webView.goToURL (juce::WebBrowserComponent::getResourceProviderRoot());

    setResizable (true, true);
    setResizeLimits (1000, 600, 2560, 1600);
    setSize (1400, 800);

    startTimerHz (60);
}

IceAudioProcessorEditor::~IceAudioProcessorEditor()
{
    stopTimer();
}

void IceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff000206));
}

void IceAudioProcessorEditor::resized()
{
    webView.setBounds (getLocalBounds());
}

std::optional<juce::WebBrowserComponent::Resource> IceAudioProcessorEditor::getResource (const juce::String& url)
{
    const auto path = url.fromFirstOccurrenceOf ("/", false, false);
    if (path.isEmpty() || path == "index.html")
    {
        std::vector<std::byte> data ((size_t) BinaryData::index_htmlSize);
        std::memcpy (data.data(), BinaryData::index_html, data.size());
        return juce::WebBrowserComponent::Resource { std::move (data), "text/html" };
    }
    return std::nullopt;
}

void IceAudioProcessorEditor::timerCallback()
{
    const int n = processor.pull (tmpL.data(), tmpR.data(), (int) tmpL.size());
    if (n <= 0)
        return;

    interleaved.resize ((size_t) n * 2);
    for (int i = 0; i < n; ++i)
    {
        interleaved[(size_t) (2 * i)]     = tmpL[(size_t) i];
        interleaved[(size_t) (2 * i + 1)] = tmpR[(size_t) i];
    }

    auto* obj = new juce::DynamicObject();
    obj->setProperty ("sr", processor.sampleRate.load());
    obj->setProperty ("n", n);
    obj->setProperty ("d", juce::Base64::toBase64 (interleaved.data(), interleaved.size() * sizeof (float)));
    webView.emitEventIfBrowserIsVisible ("audio", juce::var (obj));
}
