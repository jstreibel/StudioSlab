//
// Created by joao on 7/6/24.
//

#ifndef STUDIOSLAB_OSCILLONPLOTTING_H
#define STUDIOSLAB_OSCILLONPLOTTING_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "../../OscViewer/OscViewer.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticOscillon_1plus1d.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

namespace Studios {

    class OscillonPlotting : public Studios::Fields::OscViewer {
        using AnalyticOscillon = Slab::Math::R2toR::AnalyticOscillon_1plus1d;
        using Function = Slab::Math::R2toR::NumericFunction_CPU;
        using Parameters = Slab::Math::R2toR::AnalyticOscillon_1plus1d::OscillonParameters;

        Slab::Count n_oscillons = 10;

        int seed                = 1;
        float l_std        = 1./10;
        Slab::Resolution N      = 100;
        Slab::Resolution M      = 100;
        Slab::Real L            =    10.0;
        Slab::Real t            =    10.0;
        Slab::Real x_min        = -L/2;
        Slab::Real t_min        = -t/2;

        Parameters osc_params;
        Slab::Pointer<Function> rendered;

        void setupOscillons();
    public:
        OscillonPlotting();

        void draw() override;
    };

}

#endif //STUDIOSLAB_OSCILLONPLOTTING_H
