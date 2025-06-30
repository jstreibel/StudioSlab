//
// Created by joao on 8/09/23.
//

#include "BezierTests.h"

#include "Math/Function/NativeFunction.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"

#include "3rdParty/ImGui.h"
#include "Graphics/Modules/Animator/Animator.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Tests {
    using namespace Slab;

    double startValue=0;
    double endValue=1;

    float param1 = .25;
    float param2 = .75;
    float AnimTimeSeconds = 1.0;

    double CubicBezierInterpolation(DevFloat t) {
        // Cubic Bézier curve with control points P0, P1, P2, P3

        double P0 = startValue;
        double P1 = startValue + (endValue - startValue) * (double)param1;
        double P2 = startValue + (endValue - startValue) * (double)param2;
        double P3 = endValue;

        // Cubic Bezier formula
        return std::pow(1 - t, 3) * P0 + 3 * std::pow(1 - t, 2) * t * P1 + 3 * (1 - t) * std::pow(t, 2) * P2 + std::pow(t, 3) * P3;
    }


    BezierTests::BezierTests(const Pointer<Graphics::SlabImGuiContext>& GuiContext)
    : FSlabWindow(Config("Bezier Tests"))
    , Graph("Graph", GuiContext) {
        param1 = static_cast<float>(Graphics::Animator::GetBezierParams().first);
        param2 = static_cast<float>(Graphics::Animator::GetBezierParams().second);
        AnimTimeSeconds = Graph.GetAnimationTime();

        static Math::Base::NativeFunction<Math::RtoR::Function> s_function(CubicBezierInterpolation);
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        Graphics::Plotter::AddRtoRFunction(Naked(Graph), Naked(s_function), style, "Bezier");

        fix lim = 1.e3;
        static Math::PointSet vertLine1{{{0.0, -lim}, {0.0, lim}}};
        static Math::PointSet vertLine2{{{1.0, -lim}, {1.0, lim}}};
        style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        Graphics::Plotter::AddPointSet(Naked(Graph), Naked(vertLine1), style, "", false);
        Graphics::Plotter::AddPointSet(Naked(Graph), Naked(vertLine2), style, "", false);

        style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[2];
        style.setPrimitive(Slab::Graphics::Point);
        style.thickness = 10;
        Graphics::Plotter::AddPointSet(Naked(Graph), Naked(CurrentPoint), style, "", false);

        Graph.GetRegion().setLimits(-.2, 1.2, -.5, 1.5);
    }

    void BezierTests::ImmediateDraw() {
        FSlabWindow::ImmediateDraw();

        CurrentPoint.clear();
        const auto &GraphRegion = Graph.GetRegion();
        if(Graphics::Animator::Contains(*GraphRegion.getReference_xMin())){
            auto &Animation = Graphics::Animator::Get(*GraphRegion.getReference_xMin());
            fix t = Animation.Timer.GetElapsedTime_Seconds() / Animation.TimeInSeconds;
            CurrentPoint.AddPoint({t, CubicBezierInterpolation(t)});
        }

        if(Graphics::Animator::Contains(*GraphRegion.getReference_xMax())){
            auto &Animation = Graphics::Animator::Get(*GraphRegion.getReference_xMax());
            fix t = Animation.Timer.GetElapsedTime_Seconds() / Animation.TimeInSeconds;
            CurrentPoint.AddPoint({t, CubicBezierInterpolation(t)});
        }

        if(Graphics::Animator::Contains(*GraphRegion.getReference_yMin())){
            auto &Animation = Graphics::Animator::Get(*GraphRegion.getReference_yMin());
            fix t = Animation.Timer.GetElapsedTime_Seconds() / Animation.TimeInSeconds;
            CurrentPoint.AddPoint({t, CubicBezierInterpolation(t)});
        }

        if(Graphics::Animator::Contains(*GraphRegion.getReference_yMax())){
            auto &Animation = Graphics::Animator::Get(*GraphRegion.getReference_yMax());
            fix t = Animation.Timer.GetElapsedTime_Seconds() / Animation.TimeInSeconds;
            CurrentPoint.AddPoint({t, CubicBezierInterpolation(t)});
        }

        Graph.ImmediateDraw();
        Stats.ImmediateDraw();

    }

    void BezierTests::RegisterDeferredDrawCalls()
    {
        FSlabWindow::RegisterDeferredDrawCalls();

        Stats.Set_x(this->Get_x()-100);
        Stats.Set_y(this->Get_y());

        Stats.AddExternalDraw([this]{
            if(ImGui::SliderFloat("param1", &param1, -1, 2)
             | ImGui::SliderFloat("param2", &param2, -1, 2)
             | ImGui::SliderFloat("time", &AnimTimeSeconds, 0.1, 5)){
                Graphics::Animator::SetBezierParams(param1, param2);
                Graph.SetAnimationTime(AnimTimeSeconds);
            }});

        Stats.RegisterDeferredDrawCalls();

        Graph.RegisterDeferredDrawCalls();
    }
} // Tests