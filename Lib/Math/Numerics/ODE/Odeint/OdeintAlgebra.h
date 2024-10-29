//
// Created by joao on 6/1/24.
//

#ifndef STUDIOSLAB_ODEINTALGEBRA_H
#define STUDIOSLAB_ODEINTALGEBRA_H


#include <functional>
#include <memory>

#include "OdeintState.h"


namespace Slab::Math::Odeint {



    struct OdeintAlgebra {
        template<class State, class Operation>
        static void for_each1(State &s, Operation op) {
            s.applyOperation1(op);
        }

        template<class State, class Operation>
        static void for_each2(State &s1,
                              const State &s2,
                              Operation op) {
            s1.applyOperation2(s2, op);
        }

        template<class State, class Operation>
        static void for_each3(State &s1,
                              const State &s2,
                              const State &s3,
                              Operation op) {
            s1.applyOperation3(s2, s3, op);
        }

        template<class State, class Operation>
        static void for_each4(State &s1,
                              const State &s2,
                              const State &s3,
                              const State &s4,
                              Operation op) {
            s1.applyOperation4(s2, s3, s4, op);
        }

        template<class State, class Operation>
        static void for_each5(State &s1,
                              const State &s2,
                              const State &s3,
                              const State &s4,
                              const State &s5,
                              Operation op) {
            s1.applyOperation5(s2, s3, s4, s5, op);
        }

        template<class State, class Operation>
        static void for_each6(State &s1,
                              const State &s2,
                              const State &s3,
                              const State &s4,
                              const State &s5,
                              const State &s6,
                              Operation op) {
            s1.applyOperation6(s2, s3, s4, s5, s6, op);
        }
    };

} // Slab::Math::Odeint

#endif //STUDIOSLAB_ODEINTALGEBRA_H
