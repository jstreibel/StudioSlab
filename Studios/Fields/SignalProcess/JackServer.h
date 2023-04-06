//
// Created by joao on 4/2/23.
//

#ifndef STUDIOSLAB_JACKSERVER_H
#define STUDIOSLAB_JACKSERVER_H


#include "Common/CoreNativeTypes.h"


#include <jack/jack.h>
#include <queue>

int processJack(jack_nframes_t nframes, void *arg);
void testJack(jack_nframes_t nframes);
void bypassJack(jack_nframes_t nframes);

class JackServer {
    jack_client_t *client;
    jack_port_t *output_processed_port;
    jack_port_t *output_bypass_port;
    jack_port_t *output_440Hz_test_port;
    jack_port_t *input_port;

    const int buffer_size = 2048;
    jack_default_audio_sample_t* input_buffer = new jack_default_audio_sample_t[buffer_size];
    bool inputBufferIsUpdated = false;
    size_t totalInputBufferUpdates = 0;

    std::vector<Real> dataToOutput;
    std::vector<Real> lastOutputData;
    std::mutex dataMutex;
    int totalLastOutputProcessedSamples, nframes_jack;
    enum SamplingFlags {UnderSampled, OverSampled, EvenSampled} samplingFlag = EvenSampled;

    JackServer();

    void updateInputBuffer(jack_nframes_t nframes);

public:
    static JackServer* GetInstance();

    size_t getInputBufferUpdateCount() { return totalInputBufferUpdates; };

    std::vector<Real> &getDataToOutput() { return dataToOutput; }
    std::vector<Real> &getLastOutputData() { return lastOutputData; }

    int getInputBufferSize();
    jack_default_audio_sample_t *getInputBuffer();
    jack_port_t *getInputPort();
    jack_port_t *getOutputPort();

    void operator << (Real value);

    int getLastOutputProcessedSamples() const {return totalLastOutputProcessedSamples; }
    jack_nframes_t getnframes() const {return nframes_jack; }
    bool isSubSampled() {return samplingFlag == UnderSampled; }

    friend int processJack(jack_nframes_t nframes, void *arg);
    friend void testJack(jack_nframes_t nframes);
    friend void bypassJack(jack_nframes_t nframes);
    friend void outputProbedData(jack_nframes_t nframes);


};


#endif //STUDIOSLAB_JACKSERVER_H
