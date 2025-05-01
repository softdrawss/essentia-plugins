/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RMSAudioProcessor::RMSAudioProcessor()
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

RMSAudioProcessor::~RMSAudioProcessor()
{
    cleanupEssentia();
}

//==============================================================================
const juce::String RMSAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RMSAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RMSAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RMSAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RMSAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RMSAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RMSAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RMSAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RMSAudioProcessor::getProgramName (int index)
{
    return {};
}

void RMSAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void RMSAudioProcessor::initializeEssentiaAlgorithms(int sampleRate, int frameSize)
{
    essentia::init();
    
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    // check the parameters for each algo
    rms = factory.create("RMS");

    // connect algorithm I/Os
    rms->input("array").set(essentiaBuffer);
    rms->output("rms").set(rmsValue);
}

void RMSAudioProcessor::connectBufferToAlgorithms()
{
    rms->input("frame").set(essentiaBuffer);
    rms->output("frame").set(rmsValue);
}

std::vector<float> RMSAudioProcessor::applyZeroPadding(juce::AudioBuffer<float>& buffer, int maxSampleSize)
{
    
    float* start = buffer.getWritePointer(0); // get the pointer to the first sample of the first channel
    int size = buffer.getNumSamples();
    if (size < maxSampleSize){
        // get an array of padded zeros
        int padded_zeros_size =  maxSampleSize - size;
        std::vector<float> audio_buffer_vec(start, start + size); // this will copy the data as a vector
        // append array of padded zeros to audio_buffer
        for(int i=0; i < padded_zeros_size; i++){
            audio_buffer_vec.push_back(0.0f);
        }
        return audio_buffer_vec;

    }
    else{
        std::vector<float> audio_buffer_vec(start, start + maxSampleSize);
        return audio_buffer_vec;
    }

}

void RMSAudioProcessor::loadEssentiaBuffer(std::vector<float> buffer)
{
    std::vector<essentia::Real> audio_buffer;
    for(auto sample : buffer){
        audio_buffer.push_back(sample);
    }
    essentiaBuffer = audio_buffer;
};

void RMSAudioProcessor::computeEssentiaAlgorithms()
{
    rms->compute();
}

void RMSAudioProcessor::cleanupEssentia() {
    essentiaBuffer.clear();
    essentia::shutdown();
    //delete rms;
    //delete energy;
}

//==============================================================================
void RMSAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    initializeEssentiaAlgorithms((int)sampleRate, samplesPerBlock);
}

void RMSAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RMSAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void RMSAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    std::vector<float> audioBuffer = applyZeroPadding(buffer, maxSampleSize);
    loadEssentiaBuffer(audioBuffer);
    computeEssentiaAlgorithms();
}

//==============================================================================
bool RMSAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RMSAudioProcessor::createEditor()
{
    return new RMSAudioProcessorEditor (*this);
}

//==============================================================================
void RMSAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RMSAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RMSAudioProcessor();
}
