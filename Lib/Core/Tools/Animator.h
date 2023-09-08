//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_ANIMATOR_H
#define STUDIOSLAB_ANIMATOR_H


#include "ThreadPool.h"

namespace Core {
    namespace Graphics {

        class Animator {
            static Animator& Instance();
            Animator() {}

            struct Animation {
                double targetValue;
                double timeInSeconds;
                std::chrono::steady_clock::time_point startTime;
            };

            std::unordered_map<double*, Animation> animations;
            static double cubicBezierInterpolation(double startValue, double endValue, double t);

        public:

            static void Add(double& variable, double targetValue, double timeInSeconds);

            static bool Contains(const double &variable);

            static void Update();

        };

    } // Core
} // Graphics

#endif //STUDIOSLAB_ANIMATOR_H
