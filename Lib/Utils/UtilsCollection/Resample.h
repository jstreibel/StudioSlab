//
// Created by joao on 4/4/23.
//

#ifndef STUDIOSLAB_RESAMPLE_H
#define STUDIOSLAB_RESAMPLE_H

#include <vector>
// #include "3rdParty/libresample/include/libresample.h"


namespace Studios {
    namespace Utils {
        std::vector<float> resample(std::vector<float> in, int output_size);
    }
}




/*
std::vector<double> resample(const std::vector<double>& input, int output_size)
{
    const int input_size = input.size();
    const double input_rate = input_size;
    const double output_rate = output_size;
    std::vector<double> output(output_size); // allocate output buffer

    // create resampler object

    rs_data* rs = rs_create(input_rate, output_rate, 1, NULL);
    if (!rs) {
        throw std::runtime_error("Failed to create libresample resampler");
    }

    // resample input buffer to output buffer
    int output_pos = 0;
    for (int input_pos = 0; input_pos < input_size; input_pos += rs->input_frames) {
        const int input_frames = std::min(input_size - input_pos, rs->input_frames);
        const int output_frames = rs_process(rs, 0, input.data() + input_pos, input_frames, output.data() + output_pos, output_size - output_pos);
        output_pos += output_frames;
    }

    // cleanup
    rs_delete(rs);

    return output;
}


#include <soxr.h>
#include <stdexcept>

std::vector<double> resample(const std::vector<double>& input, double input_rate, double output_rate)
{
    const int input_size = input.size();
    std::vector<double> output(input_size * output_rate / input_rate); // allocate output buffer

    // create resampler object
    soxr_error_t err;
    auto soxr = soxr_create(input_rate, output_rate, 1, &err, NULL, NULL, NULL);
    if (err) {
        throw std::runtime_error("Failed to create SOXR resampler: " + std::string(err));
    }

    // resample input buffer to output buffer
    const soxr_datatype_t datatype = SOXR_FLOAT64_I;
    soxr_process(soxr, input.data(), input_size, NULL, output.data(), output.size(), NULL);

    // cleanup
    soxr_delete(soxr);

    return output;
}

/*
std::vector<int> grow(const std::vector<int>& v, std::size_t k)
{
    if (v.empty()) {
        return {};
    }

    std::vector<int> res(v.size() * (k - 1) + 1);

    for (std::size_t i = 0; i + 1 < v.size(); ++i) {
        for (std::size_t j = 0; j != k; ++j) {
            res[i * k + j] = std::lerp(v[i], v[i + 1], float(j) / k);
        }
    }
    res.back() = v.back();
    return res;
}*/


#endif //STUDIOSLAB_RESAMPLE_H
