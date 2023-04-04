//
// Created by joao on 4/2/23.
//

#include "JackServer.h"
#include "Common/STDLibInclude.h"
#include "Common/Utils.h"
#include <iostream>
#include <cmath>
#include <csignal>
#include <cstring>

JackServer *jack = nullptr;


float testWaveFreq = 1.0;


void error(const char *msg){ std::cout << "Jack error: \"" << msg << "\"" << std::endl; }

void testJack(jack_nframes_t nframes);
void bypassJack(jack_nframes_t nframes);

int processJack(jack_nframes_t nframes, void *arg){
    jack->updateInputBuffer(nframes);

    bypassJack(nframes);
    testJack(nframes);


    // PROCESS
    auto &processedSamples = jack->dataToOutput;
    jack->samplingFlag = (nframes > processedSamples.size()) ? JackServer::UnderSampled :
                         (nframes < processedSamples.size()) ? JackServer::OverSampled :
                                                      JackServer::EvenSampled ;

    auto output_processed_port = jack->getOutputPort();

    jack_default_audio_sample_t *out =
            (jack_default_audio_sample_t *) jack_port_get_buffer(output_processed_port, nframes);

    auto nsamples = processedSamples.size();
    if(nsamples) {
        auto ds = 1. / nframes;
        for (int i = 0; i < nframes; ++i) {
            const auto s = i * ds;
            auto j = s * nsamples;

            out[i] = (jack_default_audio_sample_t) processedSamples[j];
        }
    }

    jack->lastOutputProcessedSamples = nsamples;
    processedSamples.clear();

    return 0;
}


JackServer::JackServer() {
    if(jack != nullptr) throw "Fields' Jack server already initialized";

    jack_set_error_function(error);
    JackStatus status;

    client = jack_client_open("Signals through fields ~~~~~~~~~~~", JackNullOption, &status);

    auto testPortName = (ToString(testWaveFreq) + "Hz test wave").c_str();

    output_processed_port =
            jack_port_register(client, "Propagated", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_bypass_port =
            jack_port_register(client, "Bypass", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_440Hz_test_port =
            jack_port_register(client, testPortName,
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

    std::vector<String> connect_to_processed_output = {"spectrum_analyzer_x1:in0",
                                                       "Built-in Audio Analog Stereo:playback_FL",
                                                       "Built-in Audio Analog Stereo:playback_FR"};

    std::vector<String> connect_to_input =     {/*"C-1U Digital Stereo (IEC958):capture_FL",
                                                "C-1U Digital Stereo (IEC958):capture_FR",
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

void JackServer::updateInputBuffer(jack_nframes_t nframes) {
    assert(nframes<=buffer_size);

    this->nframes_jack = nframes;

    auto jack_input_buffer = (jack_default_audio_sample_t*) jack_port_get_buffer(getInputPort(), nframes);
    memcpy(input_buffer, jack_input_buffer, sizeof(jack_default_audio_sample_t) * nframes);

    totalInputBufferUpdates++;
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

void JackServer::operator<<(Real value) { dataToOutput.emplace_back(value); }



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


