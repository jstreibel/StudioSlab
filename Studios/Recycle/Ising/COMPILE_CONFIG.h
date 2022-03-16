//
// Created by joao on 30/07/2021.
//

#ifndef ISING_COMPILE_CONFIG_H
#define ISING_COMPILE_CONFIG_H


#define VISUAL_OUTPUT 1
#define CONSOLE_OUTPUT 2

#define OUTPUT_MODE VISUAL_OUTPUT


#define USE_LOOKUP_TABLE_FOR_DELTA_E true


// Esse true pode ser trocado pra false a vontade sem danos
#define FULLSCREEN false

// 16msec/frame => 60Hz
#define FPS 60
#define FRAME_INTERVAL_MSEC (1000./FPS)
#define LIMIT_SIM_SPEED true


#define MAX_BITSET_SIZE (4096*4096) // max Ising network size



#endif //ISING_COMPILE_CONFIG_H
