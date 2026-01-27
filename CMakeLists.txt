cmake_minimum_required(VERSION 3.15)

set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Deployment target for macOS" FORCE)

project(EA_PURE_COMPRESSOR VERSION 0.0.1)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(EA_PURE_COMPRESSOR
    COMPANY_NAME "EMU AUDIO"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE EmAu
    PLUGIN_CODE EaPC
    FORMATS AU VST3
    PRODUCT_NAME "EA PURE COMPRESSOR"
    BUNDLE_ID "com.emuaudio.eapurecompressor"
    VERSION 0.0.1
)

target_sources(EA_PURE_COMPRESSOR
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/DSP/CompressorEngine.h
        Source/DSP/CompressorEngine.cpp
        Source/DSP/CoreProtect.h
        Source/DSP/CoreProtect.cpp
        Source/DSP/CrystallineSaturation.h
        Source/DSP/CrystallineSaturation.cpp
)

target_compile_definitions(EA_PURE_COMPRESSOR
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_add_binary_data(PluginAssets
    SOURCES
        Resources/background.png
        Resources/knob_metal.png
        Resources/knob_black.png
)

juce_generate_juce_header(EA_PURE_COMPRESSOR)

target_link_libraries(EA_PURE_COMPRESSOR
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        PluginAssets
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
