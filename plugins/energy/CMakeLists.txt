project(EnergyPlugin VERSION 0.1.0)
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

juce_add_plugin(
    ${PROJECT_NAME}
    COMPANY_NAME "Music Technology Group"
    BUNDLE_ID "com.MusicTechnologyGroup.EnergyPlugin"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    # COPY_PLUGIN_AFTER_BUILD ${INTALL_PLUGIN_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE Mtge
    PLUGIN_CODE Eeng
    FORMATS VST3
    PRODUCT_NAME "Energy"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
)

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
