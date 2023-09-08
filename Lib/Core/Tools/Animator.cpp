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

            animations[&variable] = {targetValue, timeInSeconds, std::chrono::steady_clock::now()};
        }

        void Animator::Update() {
            auto now = std::chrono::steady_clock::now();

            auto &animations = Animator::Instance().animations;

            for (auto it = animations.begin(); it != animations.end();) {
                auto& [var, anim] = *it;
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - anim.startTime).count() / 1000.0;

                if (elapsed >= anim.timeInSeconds) {
                    *var = anim.targetValue;
                    it = animations.erase(it);  // Remove the completed animation
                } else {
                    *var = cubicBezierInterpolation(*var, anim.targetValue, elapsed / anim.timeInSeconds);
                    ++it;
                }
            }

        }

        double Animator::cubicBezierInterpolation(double startValue, double endValue, double t) {
            // Cubic Bezier curve with control points P0, P1, P2, P3
            double P0 = startValue;
            double P1 = startValue + (endValue - startValue) * 0.25;
            double P2 = startValue + (endValue - startValue) * 0.75;
            double P3 = endValue;

            // Cubic Bezier formula
            return std::pow(1 - t, 3) * P0 + 3 * std::pow(1 - t, 2) * t * P1 + 3 * (1 - t) * std::pow(t, 2) * P2 + std::pow(t, 3) * P3;
        }

        bool Animator::Contains(const double &variable) {
            return Animator::Instance().animations.contains(const_cast<double*>(&variable));
        }
    } // Core
} // Graphics