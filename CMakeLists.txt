cmake_minimum_required(VERSION 3.22)

project(Synth VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SYNTH_BUILD_HOST "Build JUCE AudioPluginHost for local plugin testing" ON)

add_subdirectory(external/JUCE)

juce_add_plugin(Synth
    COMPANY_NAME "Synth"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE SyNt
    PLUGIN_CODE Syn1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Synth")

target_sources(Synth
    PRIVATE
        Source/PluginEditor.cpp
        Source/PluginProcessor.cpp)

target_compile_definitions(Synth
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Synth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

if(SYNTH_BUILD_HOST)
    add_subdirectory(external/JUCE/extras/AudioPluginHost)
endif()
