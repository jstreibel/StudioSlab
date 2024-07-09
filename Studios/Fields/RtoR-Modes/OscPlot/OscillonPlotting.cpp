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
        Slab::Real L;

    public:
        PeriodicInX(Slab::Pointer<R2Function> func, Slab::Real L)
        : function(func), L(L) { }

        Slab::Real operator()(Slab::Math::Real2D r) const override {
            auto L_vec = Slab::Math::Real2D(L, 0);

            return (*function)(r) + (*function)(r+L_vec) + (*function)(r-L_vec);
        }
    };

    OscillonPlotting::OscillonPlotting()
    : Studios::Fields::OscViewer()
    {
        fix l = 1.0;
        osc_params = Parameters{-l/2, l, .0, .0, .0};
        setupOscillons();
    }

    void OscillonPlotting::draw() {
        guiWindow.begin();

        if(ImGui::CollapsingHeader("Oscillon", ImGuiTreeNodeFlags_Framed)) {
            // auto v = (float)osc_params.v;
            // auto u = (float)osc_params.u;
            // auto alpha = (float)osc_params.alpha;
            auto l = (float)osc_params.l;
            auto n = (int)n_oscillons;
            auto N_ = (int)N;
            auto M_ = (int)M;
            auto L_ = (float)L;
            auto t_ = (float)t;

            if(ImGui::SliderInt("seed", &seed, 1, 64*1024-1)
             | ImGui::SliderFloat("L", &L_, 2.0, 50.0)
             | ImGui::SliderFloat("t", &t_, 2.0, 50.0)
             | ImGui::SliderInt("n", &n, 1, 1000)
             | ImGui::SliderInt("N", &N_, 100, 2000)
             | ImGui::SliderInt("M", &M_, 100, 2000)
          // | ImGui::SliderFloat("α", &alpha, 0, l)
          // | ImGui::SliderFloat("u", &u, -c_max, c_max)
          // | ImGui::SliderFloat("v", &v, -c_max, c_max)
             | ImGui::SliderFloat("λ", &l, 1e-2f, 1e1f)
             | ImGui::DragFloat("λ_std", &l_std, l*1e-3f, 1e-3f, 1e1f)) {
                L = L_;
                t = t_;

                x_min = -L/2;
                t_min = -t/2;

                // osc_params.alpha = alpha;
                // osc_params.v = v;
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

        Slab::RandUtils::SeedUniformReal(seed);
        auto Rand = Slab::RandUtils::RandomUniformReal;

        fix c_max = .99f;
        FunctionSum many_osc;
        auto l = osc_params.l;
        auto l_vec = Slab::RandUtils::GenerateLognormalValues(n_oscillons, osc_params.l, l_std, seed);

        for(auto i=0; i<n_oscillons; ++i) {
            fix u = Rand(-c_max, c_max);
            fix inv_gamma = sqrt(1-u*u);
            osc_params.x0       = Rand(x_min - L / 2, x_min + L + L / 2 - inv_gamma * osc_params.l);
            osc_params.u        = u;
            osc_params.v        = 0;//Slab::RandUtils::random(-c_max,c_max);
            osc_params.alpha    = Rand(0, osc_params.l);
            osc_params.l = l_vec[i];

            many_osc += AnalyticOscillon (osc_params);
        }
        osc_params.l = l;

        PeriodicInX periodic(Slab::Naked(many_osc), L);
        rendered = Slab::New<Slab::Math::R2toR::NumericFunction_CPU>(N,                 M,
                                                                     x_min,             t_min,
                                                                     L/(Slab::Real)N,   t/(Slab::Real)M);
        Slab::Math::R2toR::R2toRFunctionRenderer::renderToDiscrete(periodic, rendered);

        this->setFunction(rendered);
    }

}