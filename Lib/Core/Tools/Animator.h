//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_ANIMATOR_H
#define STUDIOSLAB_ANIMATOR_H


#include "Utils/Timer.h"

#include <unordered_map>

namespace Core {
    namespace Graphics {

        struct Animation {
            double initialValue;
            double targetValue;
            double timeInSeconds;
            Timer timer;
        };

        class Animator {
            static Animator& Instance();
            Animator() {}

            std::unordered_map<double*, Animation> animations;
            static double cubicBezierInterpolation(double startValue, double endValue, double t);

            double p1=1.0,p2=1.0;
        public:


            static void Add(double& variable, double targetValue, double timeInSeconds);

            static bool Contains(const double &variable);

            static auto Get(const double &variable) -> const Animation&;

            static void SetBezierParams(double p1, double p2);

            static auto GetBezierParams() -> std::pair<double, double>;

            static void Update();

        };

    } // Core
} // Graphics

#endif //STUDIOSLAB_ANIMATOR_H