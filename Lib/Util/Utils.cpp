//
// Created by joao on 13/09/2019.
//

#include "Utils.h"
#include "omp.h"

IntPair getBeginAndEndForThreadedFor(int N) {
    const int nThreads = omp_get_num_threads();
    const int myThreadRank = omp_get_thread_num();
    const int block_size = N/nThreads;
    const int begin = myThreadRank * block_size;
    const int end = begin + block_size;

    return {begin, end};
}

void PrintThere(int x, int y, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("\033[%d;%dH", x, y);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

char GetDensityChar(float dens){
    const int N = 70;
    const char* ascii_gs = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`\'. ";

    int loc = (N-dens*N);
    loc = loc>N-1?N-1:loc<0?0:loc;

    return ascii_gs[loc];
}

void PrintDensityThere(int x, int y, float dens) {
    PrintThere(x, y, "%c", GetDensityChar(dens));
}
