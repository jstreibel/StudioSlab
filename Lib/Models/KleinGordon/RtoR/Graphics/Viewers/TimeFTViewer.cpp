//
// Created by joao on 10/11/24.
//

#include "TimeFTViewer.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Model/Operators/Integral.h"

#include "Graphics/Graph/Plotter.h"
#include "Graphics/DataViewers/Viewers/FourierViewer.h"
#include "Graphics/Graph/PlotThemeManager.h"

namespace Slab::Models::KGRtoR {

    TimeFTViewer::TimeFTViewer(const Pointer<Graphics::GUIWindow> &pointer,
                               const Pointer<R2toR::NumericFunction> &func,
                               const Pointer<R2toR::NumericFunction> &ddtFunc)
    : KGViewer(pointer, func, ddtFunc) {
        using Plotter = Graphics::Plotter;
        using Themes = Graphics::PlotThemeManager;

        auto window = New<Graphics::PlottingWindow>("Time-DFT space-average");
        timeDFTAverageArtist = Plotter::AddRtoRFunction(window, nullptr, Themes::GetCurrent()->funcPlotStyles[4], "⟨ℱₜ[ϕ]⟩ₓ(ω)");

        addWindow(window);

        if(func != nullptr){
            t0 = (float)func->getDomain().yMin;
            Δt = (float)func->getDomain().getLy();

            compute();
        }
    }

    void TimeFTViewer::compute() {
        // auto filtered = Graphics::FourierViewer::FilterSpace(getFunction(), t_min, t_max);

        auto function = getFunction();

        if(function == nullptr) return;

        Real t_0 = (Real)t0;
        Real t_f = t_0 + (Real)Δt;

        fix t_min = function->getDomain().yMin;

        t_0 = Common::max(t_0, t_min);
        t_f = Common::min(t_f, function->getDomain().yMax);

        fix N = function->getN();
        fix xMin = function->getDomain().xMin;
        fix dx = function->getDomain().getLx()/N;

        fix Mₜ = function->getM();
        fix dt = function->getDomain().getLy()/Mₜ;
        fix δt = t_f - t_0;
        fix M_ = (Count)floor(δt / dt);
        fix M = M_%2==0 ? M_ : M_-1;
        fix m = M/2 + 1;

        fix dk = 2 * M_PI / δt;

        auto ωSpace = New<Math::R2toR::NumericFunction_CPU>(N, m, xMin, 0, dx, dk);
        Math::RtoR::NumericFunction_CPU tempSpace(M, .0, dk*(Real)M);

        fix j₀ = floor((t_0-t_min)/dt);
        for(auto i=0; i<N; ++i){
            auto &spaceData_temp = tempSpace.getSpace().getHostData(false);

            for(auto j=0; j<M; ++j) spaceData_temp[j] = function->At(i, j₀+j);

            auto dftMagnitudes = RtoR::DFT::Compute(tempSpace).getMagnitudes();

            assert(dftMagnitudes->count()==m);

            for(auto k=0; k<m; ++k) {
                fix &pt = dftMagnitudes->getPoints()[k];
                fix A = pt.y;
                fix ω = pt.x;

                ωSpace->At(i, k) = A;
            }
        }

        Str timeInterval = ToStr(t_0) + " ≤ t ≤ " + ToStr(t_f);
        using Integral = R2toR::Integral;
        auto avg_time_dft = Integral({Integral::Config::dx, true})[ωSpace];

        timeDFTAverageArtist->setFunction(avg_time_dft);
        timeDFTAverageArtist->setLabel(Str("∫dx ϕ(ω,x), ") + timeInterval);
    }

    void TimeFTViewer::draw() {
        auto function = getFunction();
        if(function== nullptr){ WindowPanel::draw(); return;}

        beginGUI();

        if(ImGui::CollapsingHeader("t-filter, ℱₜ & ℱₜₓ")) {
            fix dt = function->getSpace().getMetaData().geth(1);
            fix t_min =(float)function->getDomain().yMin;
            fix t_max =(float)function->getDomain().yMax;

            ImGui::Checkbox("Auto update##time_dft_space_integral", &auto_update);

            ImGui::BeginDisabled(auto_update);
            if(ImGui::Button("Compute"))
                compute();
            ImGui::EndDisabled();

            if(ImGui::SliderFloat("tₘᵢₙ", &t0, t_min, t_max-Δt)
             | ImGui::SliderFloat("Δt", &Δt, 10*(float)dt, t_max-t0)) {
                if (auto_update) compute();
            }
        }

        endGUI();

        WindowPanel::draw();
    }

    void TimeFTViewer::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        Viewer::setFunction(function);

        t0 = (float)getFunction()->getDomain().yMin;
        Δt = (float)getFunction()->getDomain().getLy();

        if(isVisible()) compute();
    }

    void TimeFTViewer::setFunctionDerivative(FuncPointer pointer) {
        KGViewer::setFunctionDerivative(pointer);
    }

    Str TimeFTViewer::getName() const { return "Time-DFT space-average viewer"; }

    void TimeFTViewer::notifyBecameVisible() {
        Viewer::notifyBecameVisible();

        if(getFunction() != nullptr) compute();
    }

} // Slab::Models::KGRtoR