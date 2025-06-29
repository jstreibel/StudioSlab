//
// Created by joao on 7/6/24.
//

#include "OscillonPlotting.h"

#include <utility>

#include "3rdParty/ImGui.h"

#include "Math/Function/R2toR/Calc/R2toRFunctionRenderer.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Utils/RandUtils.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/TwoPointCorrelationViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/TimeFTViewer.h"


namespace Studios {
    using R2Function = Slab::Math::R2toR::Function;

    class PeriodicInX : public R2Function {
        Slab::Pointer<R2Function> function;
        Slab::DevFloat L;

    public:
        PeriodicInX(Slab::Pointer<R2Function> func, Slab::DevFloat L)
        : function(std::move(func)), L(L) { }

        Slab::DevFloat operator()(Slab::Math::Real2D r) const override {
            auto L_vec = Slab::Math::Real2D(L, 0);

            return (*function)(r) + (*function)(r+L_vec) + (*function)(r-L_vec);
        }
    };

    OscillonPlotting::OscillonPlotting()
    : Slab::Models::KGRtoR::KGMainViewer()
    {
        fix l = 1.0;
        osc_params = Parameters{-l/2, l, .0, .0, .0};
        setupOscillons();

        auto two_point_viewer = Slab::New<Slab::Models::KGRtoR::TwoPointCorrelationViewer_KG>(getGUIWindow());
        addKGViewer(two_point_viewer);

        auto time_ft_space_avg_viewer = Slab::New<Slab::Models::KGRtoR::TimeFTViewer>(getGUIWindow());
        addKGViewer(time_ft_space_avg_viewer);

        auto fourier_viewer = Slab::New<Slab::Graphics::FourierViewer>(getGUIWindow());
        addViewer(fourier_viewer);

        histograms_viewer = Slab::New<Slab::Models::KGRtoR::HistogramsViewer_KG>(getGUIWindow());
        addKGViewer(histograms_viewer);

        energy_viewer = Slab::New<Slab::Models::KGRtoR::EnergyViewer_KG>(getGUIWindow());
        addKGViewer(energy_viewer);
    }

    void OscillonPlotting::Draw() {
        getGUIWindow()->AddExternalDraw([this](){
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

                auto _1_m_cmax = (float)(1.0-c_max);

                if(ImGui::SliderInt("seed", &seed, 1, 64*1024-1)
                   | ImGui::SliderFloat("L", &L_, 2.0, 50.0)
                   | ImGui::SliderFloat("t", &t_, 2.0, 50.0)
                   | ImGui::DragInt("n", &n, log(1.0f+n/500.f), 1, 1000)
                   | ImGui::SliderInt("N", &N_, 100, 2000)
                   | ImGui::SliderInt("M", &M_, 100, 2000)
                   // | ImGui::SliderFloat("α", &alpha, 0, l)
                   // | ImGui::SliderFloat("u", &u, -c_max, c_max)
                   // | ImGui::SliderFloat("v", &v, -c_max, c_max)
                   | ImGui::SliderFloat("λ", &l, 1e-2f, 1e1f)
                   | ImGui::DragFloat("λ_std", &l_std, l*1e-3f, 1e-3f, 1e1f)
                   | ImGui::DragFloat("1-c_max", &_1_m_cmax, _1_m_cmax*1e-2f, 1e-6f, 1, "%.6f")) {
                    L = L_;
                    t = t_;

                    x_min = -L/2;
                    t_min = -t/2;

                    // osc_params.alpha = alpha;
                    // osc_params.v = v;
                    // osc_params.u = u;
                    osc_params.l = l;

                    n_oscillons = n;
                    N = N_%2 ? N_-1 : N_;
                    M = M_%2 ? M_-1 : M_;

                    c_max = 1.0 - (Slab::DevFloat)_1_m_cmax;

                    setupOscillons();

                }
                ImGui::Text("c_max=%f", c_max);
                ImGui::Text("render time=%f", render_time);
            }
        });

        Slab::Graphics::MainViewer::Draw();
    }

    void OscillonPlotting::setupOscillons() {
        Slab::RandUtils::SeedUniformReal(seed);
        auto Rand = Slab::RandUtils::RandomUniformReal;

        auto l = osc_params.l;
        auto l_vec = Slab::RandUtils::GenerateLognormalValues((int)n_oscillons, osc_params.l, l_std, seed);

        many_osc.clear();



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

        oscillons_dirty = true;
        ddt_oscillons_dirty = true;

        Slab::Timer timer;
        renderOscillons();
        this->setFunction(rendered_phi);
        render_time = timer.getElTime_msec();

        auto kg_viewer = getCurrentKGViewer();
        if(kg_viewer) {
            renderOscillonsTimeDerivative();
            this->setFunctionTimeDerivative(rendered_dphi);
        }
    }

    auto
    OscillonPlotting::renderManyOsc() -> Slab::Pointer<OscillonPlotting::Function> {
        PeriodicInX periodic(Slab::Naked(many_osc), L);
        auto new_rendered = Slab::Math::DataAlloc<Slab::Math::R2toR::NumericFunction_CPU>(
                periodic.Symbol() + " [rendered]",
                N,                 M,
                x_min,             t_min,
                L/(Slab::DevFloat)N,   t/(Slab::DevFloat)M);

        Slab::Math::R2toR::R2toRFunctionRenderer::renderToDiscrete(periodic, new_rendered);

        return new_rendered;
    }

    void OscillonPlotting::renderOscillons() {
        if(!oscillons_dirty) return;

        for(const auto& term : many_osc) {
            auto osc = Slab::DynamicPointerCast<AnalyticOscillon>(term);

            osc->setBit(Slab::Math::RtoR::AnalyticOscillon::phi);
        }

        rendered_phi = renderManyOsc();

        oscillons_dirty = false;
    }

    void OscillonPlotting::renderOscillonsTimeDerivative() {
        if(!ddt_oscillons_dirty) return;

        for(const auto& term : many_osc) {
            auto osc = Slab::DynamicPointerCast<AnalyticOscillon>(term);

            osc->setBit(Slab::Math::RtoR::AnalyticOscillon::dPhiDt);
        }

        rendered_dphi = renderManyOsc();

        ddt_oscillons_dirty = false;
    }

    Slab::Pointer<Slab::Math::R2toR::FNumericFunction> OscillonPlotting::getFunctionTimeDerivative() {
        renderOscillonsTimeDerivative();

        return rendered_dphi;
    }


}