//
// Created by joao on 8/09/23.
//

#include "Animator.h"
#include "Log.h"

#include <cmath>
#include <chrono>

namespace Core {
    namespace Graphics {

        Animator &Animator::Instance() {
            static Animator instance;
            return instance;
        }

        void Animator::Add(double &variable, double targetValue, double timeInSeconds) {
            auto &animations = Instance().animations;

            animations[&variable] = {variable, targetValue, timeInSeconds};
        }

        void Animator::Update() {
            auto now = std::chrono::steady_clock::now();

            auto &animations = Animator::Instance().animations;

            for (auto it = animations.begin(); it != animations.end();) {
                auto& [var, anim] = *it;
                auto t = anim.timer.getElTime_sec()/anim.timeInSeconds;

                if (t >= 1) {
                    *var = anim.targetValue;
                    it = animations.erase(it);  // Remove the completed animation
                } else {
                    fix Δv = anim.targetValue-anim.initialValue;
                    *var = anim.initialValue + Δv*cubicBezierInterpolation(0, 1, t);
                    ++it;
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
            return std::pow(1 - t, 3) * P0 + 3 * std::pow(1 - t, 2) * t * P1 + 3 * (1 - t) * std::pow(t, 2) * P2 + std::pow(t, 3) * P3;
        }

        bool Animator::Contains(const double &variable) {
            return Animator::Instance().animations.contains(const_cast<double*>(&variable));
        }

        void Animator::SetBezierParams(double p1, double p2) {
            Instance().p1 = p1;
            Instance().p2 = p2;
        }

        auto Animator::Get(const double &variable) -> const Animation & {
            return Animator::Instance().animations[const_cast<double*>(&variable)];
        }

        auto Animator::GetBezierParams() -> std::pair<double, double> {
            auto &anim = Animator::Instance();
            return {anim.p1, anim.p2};
        }
    } // Core
} // Graphics