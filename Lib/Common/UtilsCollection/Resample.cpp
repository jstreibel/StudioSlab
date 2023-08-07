//
// Created by joao on 4/4/23.
//

#include "Resample.h"



std::vector<float> Studios::Utils::resample(std::vector<float> in, int output_size) {
    // auto fs_in = in.size();
    // const int fs_out = output_size;
    // auto factor = fs_out/(float)fs_in;
    // std::vector<float> out(fs_out);
//
    // void *resample_context = resample_open(1, factor, factor);
//
    // int in_buffer_used;
    // resample_process(resample_context, factor,
    //                  &in[0], fs_in, true, &in_buffer_used,
    //                  &out[0], fs_out);
//
    // if(in_buffer_used != fs_in) throw "resampling error";
//
    // resample_close(resample_context);
//
    // return out;

    throw "Resample not implemented";
}
