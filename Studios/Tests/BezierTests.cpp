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

    double StartValue=0;
    double EndValue=1;

    float Param1 = .25;
    float Param2 = .75;
    float AnimTimeSeconds = 1.0;

    double CubicBezierInterpolation(const double t) {
        // Cubic Bézier curve with control points P0, P1, P2, P3

        const double P0 = StartValue;
        const double P1 = StartValue + (EndValue - StartValue) * static_cast<double>(Param1);
        const double P2 = StartValue + (EndValue - StartValue) * static_cast<double>(Param2);
        const double P3 = EndValue;

        // Cubic Bezier formula
        return std::pow(1 - t, 3) * P0 + 3 * std::pow(1 - t, 2) * t * P1 + 3 * (1 - t) * std::pow(t, 2) * P2 + std::pow(t, 3) * P3;
    }


    BezierTests::BezierTests(const Pointer<Graphics::FImGuiContext>& GuiContext)
    : FWindowRow("Bezier Tests")
    , Stats(Graphics::FSlabWindowConfig("Stats"))
    , Graph("Graph", GuiContext) {
        Param1 = static_cast<float>(Graphics::Animator::GetBezierParams().first);
        Param2 = static_cast<float>(Graphics::Animator::GetBezierParams().second);
        AnimTimeSeconds = Graph.GetAnimationTime();

        static Math::Base::NativeFunction<Math::RtoR::Function> s_function(CubicBezierInterpolation);
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        Graphics::Plotter::AddRtoRFunction(Naked(Graph), Naked(s_function), style, "Bezier");

        Fix Lim = 1.e3;
        static Math::PointSet vertLine1{{{0.0, -Lim}, {0.0, Lim}}};
        static Math::PointSet vertLine2{{{1.0, -Lim}, {1.0, Lim}}};
        style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        Graphics::Plotter::AddPointSet(Naked(Graph), Naked(vertLine1), style, "", false);
        Graphics::Plotter::AddPointSet(Naked(Graph), Naked(vertLine2), style, "", false);

        style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[2];
        style.setPrimitive(Slab::Graphics::Point);
        style.thickness = 10;
        Graphics::Plotter::AddPointSet(Naked(Graph), Naked(CurrentPoint), style, "", false);

        Graph.GetRegion().setLimits(-.2, 1.2, -.5, 1.5);

        AddWindow(Dummy(Graph));
    }

    void BezierTests::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
        FSlabWindow::ImmediateDraw(PlatformWindow);

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

        Graph.ImmediateDraw(PlatformWindow);
        Stats.ImmediateDraw(PlatformWindow);

    }

    void BezierTests::RegisterDeferredDrawCalls(const Graphics::FPlatformWindow& PlatformWindow)
    {
        FSlabWindow::RegisterDeferredDrawCalls(PlatformWindow);

        Stats.Set_x(this->Get_x()-100);
        Stats.Set_y(this->Get_y());

        Stats.AddExternalDraw([this]{
            if(ImGui::SliderFloat("param1", &Param1, -1, 2)
             | ImGui::SliderFloat("param2", &Param2, -1, 2)
             | ImGui::SliderFloat("time", &AnimTimeSeconds, 0.1, 5)){
                Graphics::Animator::SetBezierParams(Param1, Param2);
                Graph.SetAnimationTime(AnimTimeSeconds);
            }});

        Stats.RegisterDeferredDrawCalls(PlatformWindow);

        Graph.RegisterDeferredDrawCalls(PlatformWindow);
    }
} // Tests