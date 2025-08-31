//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_OPDEFS_H
#define STUDIOSLAB_OPDEFS_H

#define AssertSizes(a) assert((a).first().size() == (a).second().size());
#define AssertCompatibility(a, b)    \
    assert((a).Data.first.size() == (b).Data.first.size());

#define TinySetup(STATE, STATE_NAME) \
    auto &STATE_NAME = Cast(STATE);          \
    AssertCompatibility(*this, STATE_NAME);

#define SetupOperation(STATE, TYPE, NAME) \
    AssertSizes(STATE)                    \
    TYPE  q##NAME = (STATE).first();     \
    TYPE  p##NAME = (STATE).second();

//#define Setup(STATE, TYPE) \
//    Setup_WithName(STATE, TYPE, _)
    // AssertSizes(STATE)                  \
    // TYPE  q_##TYPE = (STATE).first();   \
    // TYPE  p_##TYPE = (STATE).second();

#define Operation(OP)                   \
    {                                   \
        TinySetup(eqState, state)       \
        SetupOperation(state, IN, _IN)   \
        SetupOperation(*this, OUT, _OUT) \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] OP q_IN[i];        \
            p_OUT[i] OP p_IN[i];        \
        }                               \
    }

#define StoreOperation(OP)              \
    {                                   \
        TinySetup(eqState1, state1)     \
        TinySetup(eqState2, state2)     \
        \
        SetupOperation(state1, IN, 1_IN)         \
        SetupOperation(state2, IN, 2_IN)         \
        SetupOperation(*this, OUT, _OUT)         \
        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] = q1_IN[i] OP q2_IN[i]; \
            p_OUT[i] = p1_IN[i] OP p2_IN[i]; \
        }                           \
    }

#define ScalarOperation(OP)             \
    {                                   \
        SetupOperation(*this, OUT, _OUT)         \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] OP a;              \
            p_OUT[i] OP a;              \
        }                               \
    }

#define StoreScalarOperation(OP)        \
    {                                   \
        TinySetup(eqState, state)       \
        SetupOperation(state, IN, _IN)           \
        SetupOperation(*this, OUT, _OUT)         \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] = a OP q_IN[i];    \
            p_OUT[i] = a OP p_IN[i];    \
        }                               \
    }

#endif //STUDIOSLAB_OPDEFS_H
