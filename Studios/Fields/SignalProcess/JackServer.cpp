//
// Created by joao on 4/2/23.
//



#include "JackServer.h"

#include "Common/STDLibInclude.h"
#include "Common/Utils.h"
#include "Common/UtilsCollection/Resample.h"

#include <iostream>
#include <cmath>
#include <cstring>


JackServer *jack = nullptr;


float testWaveFreq = 1.0;



void error(const char *msg){ std::cout << "Jack error: \"" << msg << "\"" << std::endl; }



void testJack(jack_nframes_t nframes);
void bypassJack(jack_nframes_t nframes);

void outputProbedData(jack_nframes_t nframes);

void recordInput();

int processJack(jack_nframes_t nframes, void *arg){
    jack->dataMutex.lock();

    bypassJack(nframes);
    testJack(nframes);

    outputProbedData(nframes);

    if(jack->isRecordingInput){
        jack->recordInputBuffer(nframes);
    }

    auto &processedSamples = jack->dataToOutput;
    const auto nsamples = processedSamples.size();
    jack->samplingFlag = (nframes > nsamples) ? JackServer::UnderSampled :
                         (nframes < nsamples) ? JackServer::OverSampled :
                                                JackServer::EvenSampled ;

    jack->totalLastOutputProcessedSamples = nsamples;

    jack->lastOutputData = processedSamples;
    processedSamples.clear();

    jack->dataMutex.unlock();

    return 0;
}


JackServer::JackServer() {
    if(jack != nullptr) throw "Fields' Jack server already initialized";

    jack_set_error_function(error);
    JackStatus status;

    client = jack_client_open("Signals through fields ~~~~~~~~~~~", JackNullOption, &status);

    auto testPortName = ToString(testWaveFreq) + "Hz test wave";

    output_processed_port =
            jack_port_register(client, "Probed", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_bypass_port =
            jack_port_register(client, "Bypass", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_440Hz_test_port =
            jack_port_register(client, testPortName.c_str(),
                               JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    input_port =
            jack_port_register(client, "In", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    if(client!=nullptr && output_processed_port != nullptr && input_port != nullptr)
        std::cout << "Jack client and output port started successfully @"
                  << jack_get_sample_rate(client) << "samples/sec" << std::endl;
    else
        throw "Jack error 1";

    if(jack_set_process_callback(client, processJack, 0))
        throw "Jack error 2";

    const char **ports = jack_get_ports(client, NULL, NULL, 0);
    if(ports == nullptr)
        throw "Jack error 3";

    std::vector<String> connect_to_processed_output = {"spectrum_analyzer_x1:in0"/*,
                                                       /*"Built-in Audio Analog Stereo:playback_FL",
                                                       "Built-in Audio Analog Stereo:playback_FR"*/};

    std::vector<String> connect_to_input =     {"C-1U Digital Stereo (IEC958):capture_FL",
                                                "C-1U Digital Stereo (IEC958):capture_FR"/*,
                                                testPortName*/};


    std::cout << "Jack ports:";
    for(auto i=0; ports[i] != nullptr; i++){
        std::cout << "\n\t" << ports[i];

        auto port = String(ports[i]);

        if(Common::contains(connect_to_processed_output, port))
            jack_connect(client, jack_port_name(output_processed_port), port.c_str());

        if(Common::contains(connect_to_input, port))
            jack_connect(client, port.c_str(), jack_port_name(input_port));

    }
    std::cout << std::endl;

    jack_free(ports);

    jack = this;

    if(jack_activate(client))
        throw "Jack error 4";
}

JackServer* JackServer::GetInstance() {
    if (jack == nullptr)
        jack = new JackServer();

    return jack;
}

int JackServer::getInputBufferSize() {
    return buffer_size;
}

void JackServer::recordInputBuffer(jack_nframes_t nframes) {
    auto jack_input_buffer = (jack_default_audio_sample_t*) jack_port_get_buffer(getInputPort(), nframes);
    memcpy(input_buffer, jack_input_buffer, sizeof(jack_default_audio_sample_t) * nframes);

    std::vector<jack_default_audio_sample_t> in(input_buffer, input_buffer + nframes);
    recordedInput.insert(recordedInput.end(), in.begin(), in.end());

    totalInputBufferUpdates++;
}

bool JackServer::toggleRecording() {
    isRecordingInput = !isRecordingInput;

    std::cout << "Jack server is " << (isRecordingInput?"":"not ") << "recording input." << std::endl;

    return isRecordingInput;
}

std::vector<float> JackServer::getRecording() {
    return recordedInput;
}


jack_default_audio_sample_t *JackServer::getInputBuffer() {
    return input_buffer;
}

jack_port_t *JackServer::getInputPort() {
    return input_port;
}

jack_port_t *JackServer::getOutputPort() {
    return output_processed_port;
}

void JackServer::operator<<(Real value) {
    dataMutex.lock();
    dataToOutput.emplace_back(value);
    dataMutex.unlock();
}



void testJack(jack_nframes_t nframes){
    auto output_port = jack->output_440Hz_test_port;

    jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer(output_port, nframes);


    // Sample rate in Hz
    const int sample_rate = 48000;

    // Frequency of the sine wave in Hz
    const float frequency = testWaveFreq;

    // Amplitude of the sine wave
    const float amplitude = 1;

    // Calculate the number of samples per cycle of the sine wave
    float samples_per_cycle = sample_rate / frequency;

    // Calculate the phase increment per sample
    float phase_inc = 2.0 * M_PI / samples_per_cycle;

    // Current phase of the sine wave
    static float phase = 0.0;

    // Fill the output buffer with samples
    for (int i = 0; i < nframes; i++) {
        // Calculate the current sample value
        float sample = amplitude * sin(phase);

        // Store the sample in the output buffer
        out[i] = sample;

        // Increment the phase for the next sample
        phase += phase_inc;
        if (phase >= 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
    }
}

void bypassJack(jack_nframes_t nframes){
    assert(nframes = jack->getInputBufferSize());

    auto input_buffer = jack->getInputBuffer();
    auto output_bypass_buffer =
            (jack_default_audio_sample_t*) jack_port_get_buffer(jack->output_bypass_port, nframes);

    memcpy(output_bypass_buffer, input_buffer, sizeof(jack_default_audio_sample_t) * nframes);
}


std::vector<float> VecDoubToVecFloat(std::vector<double> &doubles){
    const auto N = doubles.size();
    std::vector<float> floats(N);

    for(auto i=0; i<N; i++)
        floats[i] = doubles[i];

    return floats;
}

void outputProbedData(jack_nframes_t nframes) {
    auto probedSamples_floats = VecDoubToVecFloat(jack->dataToOutput);

    auto output_processed_port = jack->getOutputPort();
    auto *out = (jack_default_audio_sample_t *) jack_port_get_buffer(output_processed_port, nframes);

    const auto nsamples = probedSamples_floats.size();

    if (nsamples) {
        if (1) {
            auto to_out = Studios::Utils::resample(probedSamples_floats, nframes);

            std::copy(to_out.begin(), to_out.end(), out);
        } else {
            auto ds = 1. / nframes;
            for (int i = 0; i < nframes; ++i) {
                const auto s = i * ds;
                auto j = int(s * nsamples);

                out[i] = (jack_default_audio_sample_t) probedSamples_floats[j];
            }
        }
    }
}

