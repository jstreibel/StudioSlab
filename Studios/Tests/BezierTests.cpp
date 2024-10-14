//
// Created by joao on 8/09/23.
//

#include "BezierTests.h"

#include "Math/Function/NativeFunction.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"

#include "3rdParty/ImGui.h"
#include "Core/Backend/Modules/Animator/Animator.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Tests {
    using namespace Slab;

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
        param1 = (float)Core::Animator::GetBezierParams().first;
        param2 = (float)Core::Animator::GetBezierParams().second;
        animTimeSeconds = graph.getAnimationTime();

        addWindow(Naked(stats), Right, .15);
        addWindow(Naked(graph));

        static Math::Base::NativeFunction<Math::RtoR::Function> function(cubicBezierInterpolation);
        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        Graphics::Plotter::AddRtoRFunction(Naked(graph), Naked(function), style, "Bezier");

        fix lim = 1.e3;
        static Math::PointSet vertLine1{{{0.0, -lim}, {0.0, lim}}};
        static Math::PointSet vertLine2{{{1.0, -lim}, {1.0, lim}}};
        style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        Graphics::Plotter::AddPointSet(Naked(graph), Naked(vertLine1), style, "", false);
        Graphics::Plotter::AddPointSet(Naked(graph), Naked(vertLine2), style, "", false);

        style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[2];
        style.primitive = Slab::Graphics::Point;
        style.thickness = 10;
        Graphics::Plotter::AddPointSet(Naked(graph), Naked(currentPt), style, "", false);

        graph.getRegion().setLimits(-.2, 1.2, -.5, 1.5);
    }

    void BezierTests::draw() {
        stats.begin();
        if(ImGui::SliderFloat("param1", &param1, -1, 2)
         | ImGui::SliderFloat("param2", &param2, -1, 2)
         | ImGui::SliderFloat("time", &animTimeSeconds, 0.1, 5)){
            Core::Animator::SetBezierParams(param1, param2);
            graph.setAnimationTime(animTimeSeconds);
        }
        stats.end();

        currentPt.clear();
        auto &r = graph.getRegion();
        if(Core::Animator::Contains(*r.getReference_xMin())){
            auto &anim = Core::Animator::Get(*r.getReference_xMin());
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        if(Core::Animator::Contains(*r.getReference_xMax())){
            auto &anim = Core::Animator::Get(*r.getReference_xMax());
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        if(Core::Animator::Contains(*r.getReference_yMin())){
            auto &anim = Core::Animator::Get(*r.getReference_yMin());
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        if(Core::Animator::Contains(*r.getReference_yMax())){
            auto &anim = Core::Animator::Get(*r.getReference_yMax());
            fix t = anim.timer.getElTime_sec() / anim.timeInSeconds;
            currentPt.addPoint({t, cubicBezierInterpolation(t)});
        }

        WindowRow::draw();
    }
} // Tests