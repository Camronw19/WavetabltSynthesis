#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    //sliders
     //freqSlider 
    freqSlider.addListener(this); 
    addAndMakeVisible(freqSlider); 
    freqSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal); 
    freqSlider.setRange(1, 500, 1); 
    freqSlider.setValue(200); 
    freqSlider.setTextValueSuffix("Hz"); 
   
     //ampSlider 
    ampSlider.addListener(this); 
    addAndMakeVisible(ampSlider); 
    ampSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    ampSlider.setRange(0, .5, .001); 
    ampSlider.setValue(.125); 

    //freqLabel
    freqLabel.setText("Frequency", juce::dontSendNotification); 
    freqLabel.attachToComponent(&freqSlider, true); 

    //ampLabel
    ampLabel.setText("Amplitude", juce::dontSendNotification); 
    ampLabel.attachToComponent(&ampSlider, true); 
    






    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    frequency = freqSlider.getValue();
    amplitude = ampSlider.getValue(); 
    phase = 0; 
    wtSize = 1024;  
    currentSampleRate = sampleRate; 

    phase2 = 0; 
    frequency2 = 400; 
    increment2 = frequency2 * wtSize / currentSampleRate;
    

    //one sin wave cycle 
    for (int i = 0; i < wtSize; i++)
    {
        wavetable.insert(i, sin(2.0 * juce::double_Pi * i / wtSize));
        wavetable2.insert(i, sin(2.0 * juce::double_Pi * i / wtSize)); 
    }
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    float* const leftSpeaker = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample); 
    float* const rightSpeaker = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample); 

    

    for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        leftSpeaker[sample] = (wavetable[(int)phase] * wavetable2[(int)phase2]) * amplitude;
        rightSpeaker[sample] = (wavetable[(int)phase] * wavetable2[(int)phase2]) * amplitude;
        updateFrequency(); 
        phase2 = fmod((phase2 + increment2), wtSize);
    }
     
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
   auto r = getBounds();
   r = r.removeFromRight(getWidth() / 1.1);
    freqSlider.setBounds(r.removeFromTop(getHeight() / 2));
    ampSlider.setBounds(r); 
}

//==============================================================================

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &freqSlider)
    {
        frequency = freqSlider.getValue(); 
    }
    else if (slider == &ampSlider)
    {
        amplitude = ampSlider.getValue(); 
    }
}

void MainComponent::updateFrequency()
{
    increment = frequency * wtSize / currentSampleRate;
    phase = fmod((phase + increment), wtSize);
}


