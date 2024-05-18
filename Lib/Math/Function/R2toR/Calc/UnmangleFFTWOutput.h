//
// Created by joao on 17/05/24.
//

#ifndef STUDIOSLAB_UNMANGLEFFTWOUTPUT_H
#define STUDIOSLAB_UNMANGLEFFTWOUTPUT_H

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




#endif //STUDIOSLAB_UNMANGLEFFTWOUTPUT_H
