//
// Created by joao on 7/6/24.
//

#include "OscillonPlotting.h"

#include "3rdParty/ImGui.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticOscillon_1plus1d.h"

#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/Plotter.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Calc/R2toRFunctionRenderer.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Utils/RandUtils.h"


namespace Studios {
    using R2Function = Slab::Math::R2toR::Function;

    class PeriodicInX : public R2Function {
        Slab::Pointer<R2Function> function;
        Slab::Real x_min, L;

        Slab::Math::Real2D wrap_periodic(Slab::Math::Real2D r) const {
            fix x = x_min + fmod((r.x - x_min), L);
            fix y = r.y;

            return {x, y};
        }

    public:
        PeriodicInX(Slab::Pointer<R2Function> func, Slab::Real x_min, Slab::Real L)
        : function(func), x_min(x_min), L(L) { }

        Slab::Real operator()(Slab::Math::Real2D r) const override {
            auto x = wrap_periodic(r);
            return (*function)(x);
        }
    };

    OscillonPlotting::OscillonPlotting()
    : Studios::Fields::OscViewer()
    {
        fix l = 1.0;
        osc_params = Parameters{-l/2, -l/4, l, .0, .0, .0};
        setupOscillons();
    }

    void OscillonPlotting::draw() {
        guiWindow.begin();

        if(ImGui::CollapsingHeader("Oscillon", ImGuiTreeNodeFlags_Framed)) {
            fix c_max = .99f;

            auto v = (float)osc_params.v;
            // auto u = (float)osc_params.u;
            auto l = (float)osc_params.l;
            auto n = (int)n_oscillons;
            auto N_ = (int)N;
            auto M_ = (int)M;
            auto L_ = (float)L;
            auto t_ = (float)t;

            if(ImGui::SliderFloat("L", &L_, 2.0, 50.0)
             | ImGui::SliderFloat("t", &t_, 2.0, 50.0)
             | ImGui::SliderInt("n", &n, 1, 1000)
             | ImGui::SliderInt("N", &N_, 100, 2000)
             | ImGui::SliderInt("M", &M_, 100, 2000)
          // | ImGui::SliderFloat("u", &u, -c_max, c_max)
             | ImGui::SliderFloat("v", &v, -c_max, c_max)
             | ImGui::SliderFloat("λ", &l, 1e-2f, 1e1f)) {
                L = L_;
                t = t_;

                x_min = -L/2;
                t_min = -t/2;

                osc_params.v = v;
                // osc_params.u = u;
                osc_params.l = l;

                n_oscillons = n;
                N = N_;
                M = M_;

                setupOscillons();

                if (is_Ft_auto_updating()) computeTimeDFT();
                if (is_Ftx_auto_updating()) computeAll();
            }
        }



        guiWindow.end();

        OscViewer::draw();
    }

    void OscillonPlotting::setupOscillons() {
        using FunctionSum = Slab::Math::Base::SummableFunction<Slab::Math::Real2D, Slab::Real>;

        FunctionSum many_osc;
        for(auto i=0; i<n_oscillons; ++i) {

            osc_params.x0       = Slab::RandUtils::random(x_min-L/2, x_min+L+L/2-osc_params.l);
            osc_params.u        = Slab::RandUtils::random(-.9,.9);
            osc_params.v        = Slab::RandUtils::random(-.9,.9);
            osc_params.alpha    = Slab::RandUtils::random(0,1);

            many_osc += AnalyticOscillon (osc_params);
        }

        PeriodicInX periodic(Slab::Naked(many_osc), x_min, L);
        rendered = Slab::New<Slab::Math::R2toR::NumericFunction_CPU>(N,                 M,
                                                                     x_min,             t_min,
                                                                     L/(Slab::Real)N,   t/(Slab::Real)M);
        Slab::Math::R2toR::R2toRFunctionRenderer::renderToDiscrete(periodic, rendered);

        this->setFunction(rendered);
    }

}