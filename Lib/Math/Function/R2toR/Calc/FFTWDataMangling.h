//
// Created by joao on 17/05/24.
//

#ifndef STUDIOSLAB_FFTWDATAMANGLING_H
#define STUDIOSLAB_FFTWDATAMANGLING_H

#define NON_SYMMETRIC_DATA false
#define SYMMETRIC_DATA true


namespace Slab::Math::R2toR {

    enum DataMoveMangleMode {
        KeepArrangement,
        Mangle,
        Unmangle
    };


    void MoveData(const auto *in_data, Complex *out_data,
                  int N, int M, Real scale,
                  DataMoveMangleMode mode, bool isNSymmetric = NON_SYMMETRIC_DATA);

#define UNMANGLE_i_out \
    fix σ_i = i<=halfN ? 1 : -1; \
    fix i_out = i-1 + σ_i*halfN;

#define UNMANGLE_j_out \
    fix σ_j = j<=halfM ? 1 : -1; \
    fix j_out = j-1 + σ_j*halfM;

#define KEEP_i_out \
    fix i_out = i;

#define KEEP_j_out \
    fix j_out = j;


#ifndef DONT_FIX_OUTPUT
#define FIX_i_out \
    UNMANGLE_i_out

#define FIX_j_out \
    UNMANGLE_j_out
#else

#define FIX_i_out \
    KEEP_i_out

#define FIX_j_out \
    KEEP_j_out

#endif


/**
 * Mangles FFTW double[2] memory data from [0, ..., k, -k, ..., -1] mapping to [-k .. k].
 *
 * memory representation of std::complex<double> is essentially the same as fftw_complex
 * @param in_data
 * @param out_data
 * @param N
 * @param M
 * @param scale
 * @param isNSymmetric
 */

    static inline int MangleIndex(int i, int halfN) {
        // fix σ_i = i<=halfN ? 1 : -1;
        // return i-1 + σ_i*halfN;

        return i;
    }

    void MoveData(const auto *in_data, Complex *out_data, int N, int M, Real scale, DataMoveMangleMode mode,
                  bool isNSymmetric) {
        if (isNSymmetric) NOT_IMPLEMENTED

        fix halfN = N / 2;
        fix halfM = M / 2;

        if (mode == Unmangle || mode == Mangle) {
            for (int i = 0; i < halfN; ++i) {
                fix i_in = i;
                fix i_out = i + halfN;

                for (int j = 0; j < halfM; ++j) {
                    fix j_in = j;
                    fix j_out = j + halfM;

                    out_data[i_out + j_out * N] = scale * in_data[i_in + j_in * N];
                }

                for (int j = 0; j < halfM; ++j) {
                    fix j_in = j + halfM;
                    fix j_out = j;

                    out_data[i_out + j_out * N] = scale * in_data[i_in + j_in * N];
                }

            }

            for (int i = 0; i < halfN; ++i) {
                fix i_in = i + halfN;
                fix i_out = i;

                for (int j = 0; j < halfM; ++j) {
                    fix j_in = j;
                    fix j_out = j + halfM;

                    out_data[i_out + j_out * N] = scale * in_data[i_in + j_in * N];
                }

                for (int j = 0; j < halfM; ++j) {
                    fix j_in = j + halfM;
                    fix j_out = j;

                    out_data[i_out + j_out * N] = scale * in_data[i_in + j_in * N];
                }
            }

        } else if (mode == KeepArrangement) {
            for (int i = 0; i < N * M; ++i)
                out_data[i] = scale * in_data[i];

        } else
            NOT_IMPLEMENTED
    }


}

#endif //STUDIOSLAB_FFTWDATAMANGLING_H
