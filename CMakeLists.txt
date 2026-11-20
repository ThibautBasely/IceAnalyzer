cmake_minimum_required(VERSION 3.22)
project(IceAnalyzer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT JUCE_DIR)
  include(FetchContent)
  FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(JUCE)
else()
  add_subdirectory(${JUCE_DIR} juce)
endif()

juce_add_plugin(IceAnalyzer
  COMPANY_NAME "Thibaut Basely"
  PLUGIN_MANUFACTURER_CODE Tbas
  PLUGIN_CODE IceA
  FORMATS VST3
  PRODUCT_NAME "Ice Analyzer"
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  NEEDS_WEB_BROWSER TRUE
  NEEDS_WEBVIEW2 TRUE
  VST3_CATEGORIES Fx Analyzer)

juce_add_binary_data(IceAnalyzerData SOURCES web/index.html)

target_sources(IceAnalyzer PRIVATE src/PluginProcessor.cpp src/PluginEditor.cpp)

target_compile_definitions(IceAnalyzer PUBLIC
  JUCE_WEB_BROWSER=1
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0
  JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1)

target_link_libraries(IceAnalyzer
  PRIVATE IceAnalyzerData juce::juce_audio_utils juce::juce_gui_extra
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
