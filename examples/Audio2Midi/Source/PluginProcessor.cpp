/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RTAudio2MidiAudioProcessor::RTAudio2MidiAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

RTAudio2MidiAudioProcessor::~RTAudio2MidiAudioProcessor()
{
}

//==============================================================================
const juce::String RTAudio2MidiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RTAudio2MidiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RTAudio2MidiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RTAudio2MidiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RTAudio2MidiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RTAudio2MidiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RTAudio2MidiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RTAudio2MidiAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RTAudio2MidiAudioProcessor::getProgramName (int index)
{
    return {};
}

void RTAudio2MidiAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RTAudio2MidiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    if (!audio2Midi){
    // TODO: detect if the audio2Midi pointer it is initialized
        DBG("getting access to initialize_essentia_algorithms()");
        initialize_essentia_algorithms(sampleRate, samplesPerBlock); // init algos when it doesn't exist
    }
    else{
        DBG("getting access to configure_essentia_algorithms()");
        configure_essentia_algorithms(sampleRate, samplesPerBlock); // reset if audio2midi exists
    }
    
    /*DBG("sampleRate: " + juce::String(sampleRate));
    DBG("samplesPerBlock: " + juce::String(samplesPerBlock));*/
    
    // resize the essentia buffer to avoid allocations during the processing
    inputFrame.clear();
    inputFrame.resize(samplesPerBlock);
    
    // redefine the time step for a samplerate
    this->sampleRate = sampleRate;
    msPerSample = 1000 / sampleRate;
    
    // initialize internal timer
    //! this might fail if samplerate or block size are changed on reproducing, so if it already exists let's store the previous value to be added
    if (!currentTime)
        currentTime = 0;
}

void RTAudio2MidiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    inputFrame.clear();
    messageType.clear();
    midiNoteNumber.clear();
    timeCompensation.clear();
    cleanupEssentia();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RTAudio2MidiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RTAudio2MidiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // clear the output before adding stuff
    midiMessages.clear();
    
    // timestamp implementation from https://forum.juce.com/t/how-to-timestamp-midi-events-in-processblock-when-not-playing/11556/8
    // ask the host for the current time so we can display it...
    juce::AudioPlayHead::CurrentPositionInfo newTime;
    
    // TODO: use getPosition() instead
    if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition (newTime))
    {
        // Successfully got the current time from the host..
        lastPosInfo = newTime;
    }
    else
    {
        // If the host fails to fill-in the current time, we'll just clear it to a default..
        lastPosInfo.resetToDefault();
    }
    
    // keep track of time even whe the host isn't playing (set currentTime = 0 in prepareToPlay)
    currentTime += msPerSample * buffer.getNumSamples();
    if (newTime.isPlaying != true)
        currentTime = 0;
    // DBG("Current time: " + juce::String(currentTime));
    
    
    // TODO: estimate RMS and define a threshold for signal content detection
    // TODO: if there is no signal and the system is stopped skip the processing
    // processing preparation
    load_audio_buffer(buffer);
    
    
    // execute algorithms
    compute_algorithms();
    
    
    // TODO: check the issue we have with MIDI noise and the high performance it needs for small blocks
    // this is just to provide output when it is playing and mute when it is stopped
    if (newTime.isPlaying == true){
        
        // TODO: use rms value and threshold to silence unvoiced messages
        // TODO: define loudness threshold as parameter and control it with an slider
        // TODO: define pitch confidence threshold as parameter and control it with an slider
        // TODO: apply time compensation for midi events if needed
        // TODO: get velocity value from audio2MIDI
        
        // filter empty message outputs
        if (messageType.empty()) return;
        
        // TODO: convert RMS to velocity
        DBG("rms: " + juce::String(rms) + " - " + juce::String(messageType.size()));
        
        // process a single message
        if (messageType.size() == 1){
            
            /*DBG("msg: " + juce::String(messageType[0]) + " - midinote: " + juce::String(midiNoteNumber[0]) + " - timestamp: " + juce::String(currentTime / 1000));*/
            
            auto message = juce::MidiMessage::noteOn (10, midiNoteNumber[0], (juce::uint8) 100);
        
            if (messageType[0] == "note_off") {
                // generate noteoff message
                message = juce::MidiMessage::noteOff (10, midiNoteNumber[0], (juce::uint8) 100);
            }
            
            message.setTimeStamp(currentTime);
            auto sampleNumber =  (int) (currentTime * sampleRate);
            midiMessages.addEvent(message, sampleNumber);
        
        // process note-off and note-on message simultaneously in the same block
        } else if (messageType.size() == 2){
            // generate noteoff message
            /*DBG("msg: " + juce::String(messageType[0]) + " - midinote: " + juce::String(midiNoteNumber[0]) + " - timestamp: " + juce::String(currentTime / 1000));*/
            auto message = juce::MidiMessage::noteOff (10, midiNoteNumber[0], (juce::uint8) 100);
            message.setTimeStamp(currentTime);
            auto sampleNumber =  (int) (currentTime * sampleRate);
            midiMessages.addEvent(message, sampleNumber);
            
            // generate noteon message
            /*DBG("msg2: "+ juce::String(messageType[1]) + " - midinote: " + juce::String(midiNoteNumber[1]) + " - timestamp: " + juce::String(currentTime / 1000) );*/
            message = juce::MidiMessage::noteOn (10, midiNoteNumber[1], (juce::uint8) 100);
            message.setTimeStamp(currentTime);
            sampleNumber =  (int) (currentTime * sampleRate);
            midiMessages.addEvent(message, sampleNumber);
        }
    }
    
    // clear the block data
    inputFrame.clear();
    
    // TODO: should we reset the algorithm?
}

//==============================================================================
bool RTAudio2MidiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RTAudio2MidiAudioProcessor::createEditor()
{
    return new RTAudio2MidiAudioProcessorEditor (*this);
}

//==============================================================================
void RTAudio2MidiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RTAudio2MidiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RTAudio2MidiAudioProcessor();
}

void RTAudio2MidiAudioProcessor::initialize_essentia_algorithms(int sampleRate, int frameSize)
{
    // initialize essentia library
    essentia::init();
    AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
    
    // initialize algorithms: audio2Midi and energy
    audio2Midi = factory.create("Audio2Midi",
                                "hopSize", frameSize,
                                "sampleRate", sampleRate);

    // connect buffer to algorithms
    connectBufferToAlgorithms();
};

void RTAudio2MidiAudioProcessor::configure_essentia_algorithms(int sampleRate, int frameSize)
{
    if (!essentia::isInitialized()){
        initialize_essentia_algorithms(sampleRate, frameSize);
    }
    else {
        // reset algo first
        audio2Midi->reset();
        // update configuration in audio2Midi
        audio2Midi->configure("hopSize", frameSize,
                              "sampleRate", sampleRate);
        // TODO: here we can find define new parameters. Check audio2midi params here: https://github.com/MTG/essentia/blob/1f486843935742e2ddbd6edc7e567cc4e25b542f/src/algorithms/tonal/audio2midi.cpp#L11
        // TODO: some of the params it would be nice to have: minFrequency, maxFrequency, tuningFrequency, pitchConfidenceThreshold, loudnessThreshold, transpositionAmount, minOccurrenceRate, applyTimeCompensation
        // TODO: [ADVANCED] there are other advanced parameters like minOccurrenceRate, midiBufferDuration, minNoteChangePeriod, minOnsetCheckPeriod, minOffsetCheckPeriod that could be added if needed
    }
};

void RTAudio2MidiAudioProcessor::load_audio_buffer(juce::AudioBuffer<float> buffer)
{
    float* start = buffer.getWritePointer(0); // get the pointer to the first sample of the first channel
    int numSamples = buffer.getNumSamples();
    std::vector<float> audio_buffer_vec(start, start + numSamples);
    // TODO: look at how to load data directly from the JUCE buffer
    for(auto sample : audio_buffer_vec){
        inputFrame.push_back(sample);
    }
};

void RTAudio2MidiAudioProcessor::connectBufferToAlgorithms()
{
    // connecting audio2midi
    audio2Midi->input("frame").set(inputFrame);
    audio2Midi->output("pitch").set(pitch);
    audio2Midi->output("loudness").set(rms);
    audio2Midi->output("messageType").set(messageType);
    audio2Midi->output("midiNoteNumber").set(midiNoteNumber);
    audio2Midi->output("timeCompensation").set(timeCompensation);
}

void RTAudio2MidiAudioProcessor::compute_algorithms()
{
    audio2Midi->compute();
}

void RTAudio2MidiAudioProcessor::cleanupEssentia() {
      delete audio2Midi;
      essentia::shutdown();
    }

// TODO: we need a method to empty midi messages when the timeline is stopped
// TODO: we need a method in Audio2Midi to void the internal buffer?
