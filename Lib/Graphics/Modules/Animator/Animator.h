//
// Created by joao on 8/09/23.
//

#ifndef STUDIOSLAB_ANIMATOR_H
#define STUDIOSLAB_ANIMATOR_H


#include "Utils/Timer.h"

#include <unordered_map>
#include <functional>

namespace Slab::Graphics {

    using AnimFinishCallback = std::function<void(void)>;

    template<typename T>
    using AnimStepCallback = std::function<void(T)>;

    template <typename T>
    struct Animation {
        T initialValue;
        T targetValue;
        double TimeInSeconds;
        AnimStepCallback<T> step_callback;
        AnimFinishCallback finish_callback;

        FTimer Timer; // inicializa sozinho
    };

    template<typename T>
    using AnimationMap = std::unordered_map<TPointer<T>, Animation<T>>;

    class Animator {
        static Animator& Instance();
        Animator() = default;

        AnimationMap<double> double_animations;
        AnimationMap<int> int_animations;

        static double cubicBezierInterpolation(double startValue, double endValue, double t);

        double p1=1.0,p2=1.0;
    public:

        static void Set(double& variable, double targetValue, double timeInSeconds,
                        AnimStepCallback<double> onStep = [](double){},
                        AnimFinishCallback onFinish=[](){});

        static void Set(Int& variable, Int targetValue, double timeInSeconds,
                        AnimStepCallback<Int> onStep = [](Int){},
                        AnimFinishCallback onFinish=[](){});

        static void SetCallback(Int v0, Int targetValue, double timeInSeconds, AnimStepCallback<Int> onStep, AnimFinishCallback onFinish=[](){});

        static bool Contains(const double &variable);

        static auto Get(const double &variable) -> const Animation<double>&;

        static void SetBezierParams(double p1, double p2);

        static auto GetBezierParams() -> Pair<double, double>;

        static void Update();

    };

} // Core

#endif //STUDIOSLAB_ANIMATOR_H
