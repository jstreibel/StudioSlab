//
// Created by joao on 7/6/24.
//

#ifndef STUDIOSLAB_OSCILLONPLOTTING_H
#define STUDIOSLAB_OSCILLONPLOTTING_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticOscillon_1plus1d.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

#include "Graphics/DataViewers/Viewers/FourierViewer.h"

#include "Models/KleinGordon/RtoR/Graphics/Viewers/KGMainViewer.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/HistogramsViewer_KG.h"
#include "Models/KleinGordon/RtoR/Graphics/Viewers/EnergyViewer_KG.h"

namespace Studios {

    class OscillonPlotting : public Slab::Models::KGRtoR::KGMainViewer {
        using AnalyticOscillon = Slab::Math::R2toR::AnalyticOscillon_1plus1d;
        using Function         = Slab::Math::R2toR::NumericFunction_CPU;
        using FunctionSum = Slab::Math::Base::SummableFunction<Slab::Math::Real2D, Slab::Real>;
        using Parameters       = Slab::Math::R2toR::AnalyticOscillon_1plus1d::OscillonParameters;

        Slab::Count n_oscillons = 10;

        int seed                = 1;
        float l_std             = 1./10;
        Slab::Real c_max        = 1. - 1e-4f;
        Slab::Resolution N      = 100;
        Slab::Resolution M      = 100;
        Slab::Real L            =    10.0;
        Slab::Real t            =    10.0;
        Slab::Real x_min        = -L/2;
        Slab::Real t_min        = -t/2;
        Slab::Real render_time  = .0;

        Parameters osc_params{};
        FunctionSum many_osc;
        Slab::Pointer<Function> rendered_phi = nullptr;
        Slab::Pointer<Function> rendered_dphi= nullptr;

        Slab::Pointer<Slab::Models::KGRtoR::HistogramsViewer_KG> histograms_viewer;
        Slab::Pointer<Slab::Models::KGRtoR::EnergyViewer_KG> energy_viewer;

        bool oscillons_dirty = true;
        bool ddt_oscillons_dirty = true;
        void setupOscillons();

        Slab::Pointer<Function>
        renderManyOsc();
        void renderOscillons();
        void renderOscillonsTimeDerivative();

    public:
        OscillonPlotting();

        auto
        getFunctionTimeDerivative() -> Slab::Pointer<Slab::Math::R2toR::NumericFunction> override;
        void draw() override;
    };

}

#endif //STUDIOSLAB_OSCILLONPLOTTING_H
