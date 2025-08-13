//
// Created by joao on 8/09/23.
//

#include "Animator.h"
#include "Core/Tools/Log.h"

#include <cmath>
#include <chrono>

namespace Slab::Graphics {

    Animator &Animator::Instance() {
        static Animator instance;
        return instance;
    }

    void Animator::Set(double &variable, double targetValue, double timeInSeconds,
                       AnimStepCallback<double> onStep, AnimFinishCallback onFinish) {
        auto &animations = Instance().double_animations;

        animations[Dummy(variable)] = {variable, targetValue, timeInSeconds, onStep, onFinish};
    }

    void Animator::Set(Int &variable, Int targetValue, double timeInSeconds,
                       AnimStepCallback<Int> onStep, std::function<void(void)> onFinish) {
        auto &animations = Instance().int_animations;

        animations[Dummy(variable)] = {variable, targetValue, timeInSeconds, onStep, onFinish};
    }

    void Animator::SetCallback(Int v0, Int targetValue, double timeInSeconds, AnimStepCallback<Int> onStep, AnimFinishCallback onFinish) {
        auto &animations = Instance().int_animations;

        animations[New<Int>(v0)] = {v0, targetValue, timeInSeconds, onStep, onFinish};
    }

    void Animator::Update() {
        auto now = std::chrono::steady_clock::now();

        {
            auto &animations = Animator::Instance().double_animations;

            for (auto it = animations.begin(); it != animations.end();) {
                auto &[var, anim] = *it;
                auto t = anim.Timer.GetElapsedTime_Seconds() / anim.TimeInSeconds;

                if (t >= 1) {
                    *var = anim.targetValue;
                    anim.finish_callback();
                    it = animations.erase(it);  // Remove the completed animation
                } else {
                    fix a = anim.initialValue;
                    fix b = anim.targetValue;
                    *var = cubicBezierInterpolation(a, b, t);
                    anim.step_callback(*var);
                    ++it;
                }
            }
        }{
            auto &animations = Animator::Instance().int_animations;

            for (auto it = animations.begin(); it != animations.end();) {
                auto &[var, anim] = *it;
                auto t = anim.Timer.GetElapsedTime_Seconds() / anim.TimeInSeconds;

                if (t >= 1) {
                    *var = anim.targetValue;
                    anim.finish_callback();
                    it = animations.erase(it);  // Remove the completed animation
                } else {
                    fix a = anim.initialValue;
                    fix b = anim.targetValue;
                    *var = cubicBezierInterpolation(a, b, t);
                    anim.step_callback(*var);
                    ++it;
                }
            }
        }

    }

    double Animator::cubicBezierInterpolation(double startValue, double endValue, double t) {
        fix p1 = Animator::Instance().p1;
        fix p2 = Animator::Instance().p2;

        // Cubic Bezier curve with control points P0, P1, P2, P3
        double P0 = startValue;
        double P1 = startValue + (endValue - startValue) * p1;
        double P2 = startValue + (endValue - startValue) * p2;
        double P3 = endValue;

        // Cubic Bezier formula
        return            std::pow(1 - t, 3) * P0 +
            3 * t *       std::pow(1 - t, 2) * P1 +
            3 * (1 - t) * std::pow(t, 2)     * P2 +
                          std::pow(t, 3)     * P3;
    }

    bool Animator::Contains(const double &variable) {
        return Animator::Instance().double_animations.contains(Dummy(const_cast<double&>(variable)));
    }

    void Animator::SetBezierParams(double p1, double p2) {
        Instance().p1 = p1;
        Instance().p2 = p2;
    }

    auto Animator::Get(const double &variable) -> const Animation<double> & {
        return Animator::Instance().double_animations[Dummy(const_cast<double&>(variable))];
    }

    auto Animator::GetBezierParams() -> Pair<double, double> {
        auto &anim = Animator::Instance();
        return {anim.p1, anim.p2};
    }

} // Core