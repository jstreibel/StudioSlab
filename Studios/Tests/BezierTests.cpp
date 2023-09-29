//
// Created by joao on 8/09/23.
//

#include "BezierTests.h"

#include "Math/Function/NativeFunction.h"

#include "3rdParty/imgui/imgui.h"
#include "Core/Tools/Animator.h"

namespace Tests {
    double startValue=0;
    double endValue=1;

    float param1 = .25;
    float param2 = .75;
    float animTimeSeconds = 1.0;

    double cubicBezierInterpolation(double t) {
        // Cubic Bézier curve with control points P0, P1, P2, P3

        double P0 = startValue;
        double P1 = startValue + (endValue - startValue) * (double)param1;
        double P2 = startValue + (endValue - startValue) * (double)param2;
        double P3 = endValue;

        // Cubic Bezier formula
        return std::pow(1 - t, 3) * P0 + 3 * std::pow(1 - t, 2) * t * P1 + 3 * (1 - t) * std::pow(t, 2) * P2 + std::pow(t, 3) * P3;
    }


    BezierTests::BezierTests() {
        param1 = Core::Graphics::Animator::GetBezierParams().first;
        param2 = Core::Graphics::Animator::GetBezierParams().second;
        animTimeSeconds = graph.getAnimationTime();

        addWindow(DummyPtr(stats), Right, .15);
        addWindow(DummyPtr(graph));

        static Core::NativeFunction<RtoR::Function> function(cubicBezierInterpolation);
        graph.addFunction(&function, "Bezier");

        fix lim = 1.e3;
        static Spaces::PointSet vertLine1{{{0.0, -lim}, {0.0, lim}}};
        static Spaces::PointSet vertLine2{{{1.0, -lim}, {1.0, lim}}};
        graph.addPointSet(DummyPtr(vertLine1), Styles::GetCurrent()->funcPlotStyles[1], "", false);
        graph.addPointSet(DummyPtr(vertLine2), Styles::GetCurrent()->funcPlotStyles[1], "", false);
        auto style = Styles::GetCurrent()->funcPlotStyles[2];
        style.primitive = Styles::Point;
        style.thickness = 10;
        graph.addPointSet(DummyPtr(currentPt), style, "", false);

        graph.setLimits({-.2,1.2,-.5,1.5});

    }

    void BezierTests::draw() {
        stats.begin();
        if(ImGui::SliderFloat("param1", &param1, -1, 2)
         | ImGui::SliderFloat("param2", &param2, -1, 2)
         | ImGui::SliderFloat("time", &animTimeSeconds, 0.1, 5)){
            Core::Graphics::Animator::SetBezierParams(param1, param2);
            graph.setAnimationTime(animTimeSeconds);
        }
        stats.end();

        currentPt.clear();
        auto &r = graph.getRegion();
        if(Core::Graphics::Animator::Contains(r.xMin)){
            auto &anim = Core::Graphics::Animator::Get(r.xMin);
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        if(Core::Graphics::Animator::Contains(r.xMax)){
            auto &anim = Core::Graphics::Animator::Get(r.xMax);
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        if(Core::Graphics::Animator::Contains(r.yMin)){
            auto &anim = Core::Graphics::Animator::Get(r.yMin);
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        if(Core::Graphics::Animator::Contains(r.yMax)){
            auto &anim = Core::Graphics::Animator::Get(r.yMax);
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        WindowRow::draw();
    }
} // Tests