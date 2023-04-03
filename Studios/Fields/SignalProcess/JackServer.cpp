//
// Created by joao on 4/2/23.
//

#include "JackServer.h"
#include <iostream>
#include <cmath>
#include <csignal>
#include <cstring>

JackServer *jack = nullptr;



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

    auto ds = 1./nframes;
    auto nsamples = processedSamples.size();
    for (int i = 0; i < nframes; ++i) {
        auto j = i*ds*nsamples;
        out[i] = (jack_default_audio_sample_t) processedSamples[j];
    }

    jack->lastOutputProcessedSamples = processedSamples.size();
    processedSamples.clear();

    return 0;
}


JackServer::JackServer() {
    if(jack != nullptr) throw "Fields' Jack server already initialized";

    jack_set_error_function(error);
    JackStatus status;

    client = jack_client_open("Signals through fields", JackNullOption, &status);

    output_processed_port =
            jack_port_register(client, "Propagated", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_bypass_port =
            jack_port_register(client, "Bypass", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    output_440Hz_test_port =
            jack_port_register(client, "440Hz test wave", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
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

    /*
    if(ports[0] != nullptr) jack_connect(client, jack_port_name(output_port), ports[0]);
    if(ports[1] != nullptr) jack_connect(client, jack_port_name(output_port), ports[1]);
     */

    std::cout << "Jack ports:";
    for(auto i=0; ports[i] != nullptr; i++){
        std::cout << "\n\t" << ports[i];

        if(std::string(ports[i]) == "spectrum_analyzer_x1:in0")
            jack_connect(client, jack_port_name(output_processed_port), ports[i]);

        if(std::string(ports[i]) == "C-1U Digital Stereo (IEC958):capture_FL" ||
           std::string(ports[i]) == "C-1U Digital Stereo (IEC958):capture_FR")
            jack_connect(client, ports[i], jack_port_name(input_port));
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
    const float frequency = 440.0;

    // Amplitude of the sine wave
    const float amplitude = 0.1;

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



