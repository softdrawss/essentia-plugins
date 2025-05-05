#include "RMS/PluginProcessor.h"
#include "RMS/PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void AudioPluginAudioProcessor::initializeEssentiaAlgorithms(int sampleRate, int frameSize)
{
    juce::ignoreUnused(sampleRate, frameSize);
    essentia::init();
    
    essentia::standard::AlgorithmFactory& factory = essentia::standard::AlgorithmFactory::instance();
    
    // check the parameters for each algo
    rms = factory.create("RMS");

    // connect algorithm I/Os
    rms->input("array").set(essentiaBuffer);
    rms->output("rms").set(rmsValue);
}

void AudioPluginAudioProcessor::connectBufferToAlgorithms()
{
    rms->input("frame").set(essentiaBuffer);
    rms->output("frame").set(rmsValue);
}

std::vector<float> AudioPluginAudioProcessor::applyZeroPadding(juce::AudioBuffer<float>& buffer, int newMaxSampleSize)
{
    
    float* start = buffer.getWritePointer(0); // get the pointer to the first sample of the first channel
    int size = buffer.getNumSamples();
    if (size < newMaxSampleSize){
        // get an array of padded zeros
        int padded_zeros_size =  newMaxSampleSize - size;
        std::vector<float> audio_buffer_vec(start, start + size); // this will copy the data as a vector
        // append array of padded zeros to audio_buffer
        for(int i=0; i < padded_zeros_size; i++){
            audio_buffer_vec.push_back(0.0f);
        }
        return audio_buffer_vec;

    }
    else{
        std::vector<float> audio_buffer_vec(start, start + newMaxSampleSize);
        return audio_buffer_vec;
    }
}

void AudioPluginAudioProcessor::loadEssentiaBuffer(std::vector<float> buffer)
{
    std::vector<essentia::Real> audio_buffer;
    for(auto sample : buffer){
        audio_buffer.push_back(sample);
    }
    essentiaBuffer = audio_buffer;
}

void AudioPluginAudioProcessor::computeEssentiaAlgorithms()
{
    rms->compute();
}

void AudioPluginAudioProcessor::cleanupEssentia()
{
    // delete the algorithms
    delete rms;
    rms = nullptr;
    // shutdown essentia
    essentia::shutdown();
    // clear the buffer
    essentiaBuffer.clear();
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    initializeEssentiaAlgorithms((int)sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    cleanupEssentia();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    std::vector<float> audioBuffer = applyZeroPadding(buffer, maxSampleSize);
    loadEssentiaBuffer(audioBuffer);
    computeEssentiaAlgorithms();
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
