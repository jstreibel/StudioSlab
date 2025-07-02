//
// Created by joao on 7/10/24.
//

#include "EnergyViewer_KG.h"
#include "Math/Function/RtoR/Operations/FromR2toRNumeric.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Slab::Models {

    KGRtoR::EnergyViewer_KG::EnergyViewer_KG(const Pointer<Graphics::FGUIWindow> &gui_window)
    : KGViewer(gui_window)
    , TotalEnergiesWindow(New<Graphics::FPlot2DWindow>("Energy",      gui_window->GetGUIWindowContext()))
    , TemperaturesWindow (New<Graphics::FPlot2DWindow>("Temperature", gui_window->GetGUIWindowContext()))
    , FullHistoriesWindow(New<Graphics::FPlot2DWindow>("Histories",   gui_window->GetGUIWindowContext()))
    {
        AddWindow(TemperaturesWindow);
        AddWindow(TotalEnergiesWindow);
        AddWindow(FullHistoriesWindow, true);

        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        EnergyHistoryArtist    = Graphics::Plotter::AddPointSet(TotalEnergiesWindow, EnergyHistory,
                                                                  style, "e", true);
        KineticHistoryArtist   = Graphics::Plotter::AddPointSet(TotalEnergiesWindow, KineticHistory,
                                                                  style, "½(𝜕ₜf)²", true);
        GradHistoryArtist      = Graphics::Plotter::AddPointSet(TotalEnergiesWindow, GradHistory,
                                                                  style, "½(𝜕ₓf)²", true);
        PotentialHistoryArtist = Graphics::Plotter::AddPointSet(TotalEnergiesWindow, PotentialHistory,
                                                                  style, "|f|", true);

        Func_Artist         = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "ϕ(x,t)");
        ddtFunc_Artist      = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "T¹¹=𝜕ₜϕ");

        EnergyMapArtist    = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "T⁰⁰=e(x,t)");
        KineticMapArtist   = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "k(x,t)");
        GradMapArtist      = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "g(x,t)");
        PotentialMapArtist = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "v(x,t)");

        MomentumFlowMapArtist = Graphics::Plotter::AddR2toRFunction(FullHistoriesWindow, nullptr, "T⁰¹=-𝜕ₓϕ𝜕ₜϕ");

        EnergyMapArtist   ->SetAffectGraphRanges(true);
        KineticMapArtist  ->SetAffectGraphRanges(true);
        GradMapArtist     ->SetAffectGraphRanges(true);
        PotentialMapArtist->SetAffectGraphRanges(true);


    }

    void KGRtoR::EnergyViewer_KG::SetFunction(Pointer<Math::R2toR::FNumericFunction> function) {
        Viewer::SetFunction(function);

        Func_Artist->setFunction(getFunction());
        // f_artist->setColorMap(Graphics::ColorMaps["BrBG"]->inverse().clone());
    }

    void KGRtoR::EnergyViewer_KG::SetFunctionDerivative(FuncPointer pointer) {
        KGViewer::SetFunctionDerivative(pointer);

        ddtFunc_Artist->setFunction(pointer);
        // dfdt_artist->setColorMap(Graphics::ColorMaps["BrBG"]->brg().inverse().clone());

        if(isVisible() && areFunctionsConsistent()) UpdateEnergy();
    }

    void KGRtoR::EnergyViewer_KG::UpdateEnergy() {
        if(!areFunctionsConsistent()) return;

        auto f = getFunction();
        auto dfdt = getFunctionDerivative();

        auto H = getHamiltonian();

        auto energy_func         = DynamicPointerCast<R2toR::FNumericFunction>(f->Clone());
        auto kinetic_func        = DynamicPointerCast<R2toR::FNumericFunction>(f->Clone());
        auto grad_func           = DynamicPointerCast<R2toR::FNumericFunction>(f->Clone());
        auto potential_func      = DynamicPointerCast<R2toR::FNumericFunction>(f->Clone());

        auto momentum_flow_func  = DynamicPointerCast<R2toR::FNumericFunction>(f->Clone());

        EnergyHistory->clear();
        KineticHistory->clear();
        GradHistory->clear();
        PotentialHistory->clear();

        auto Slice = Slab::Math::RtoR::FromR2toR;

        fix N = f->getN();
        fix M = f->getM();
        fix dt = f->getDomain().getLy()/(M-1);
        fix t0 = f->getDomain().yMin;
        auto laplacian_type = Math::RtoR::NumericFunction::Standard1D_PeriodicBorder;
        for(auto j=0; j<M; ++j) {
            auto f_slice    = Slice(f,    j, laplacian_type);
            auto dfdt_slice = Slice(dfdt, j, laplacian_type);

            H.computeEnergies(*f_slice, *dfdt_slice);

            {
                fix t = j * dt + t0;
                EnergyHistory   ->AddPoint(t, H.GetTotalEnergy());
                KineticHistory  ->AddPoint(t, H.GetTotalKineticEnergy());
                GradHistory     ->AddPoint(t, H.GetTotalGradientEnergy());
                PotentialHistory->AddPoint(t, H.GetTotalPotentialEnergy());
            }

            {
                const
                double *in = &H.getEnergyDensity()->getSpace().getHostData(true)[0];
                double *out = &energy_func->At(0, j);

                memcpy(out, in, sizeof(DevFloat) * N);
            }

            {
                const
                double *in = &H.getKineticDensity()->getSpace().getHostData(true)[0];
                double *out = &kinetic_func->At(0, j);

                memcpy(out, in, sizeof(DevFloat) * N);
            }

            {
                const
                double *in = &H.getGradientDensity()->getSpace().getHostData(true)[0];
                double *out = &grad_func->At(0, j);

                memcpy(out, in, sizeof(DevFloat) * N);
            }

            {
                const
                double *in = &H.getPotentialDensity()->getSpace().getHostData(true)[0];
                double *out = &potential_func->At(0, j);

                memcpy(out, in, sizeof(DevFloat) * N);
            }
        }

        EnergyMapArtist   ->setFunction(energy_func);
        //energy_map_artist   ->setColorMap(Graphics::ColorMaps["afmhot"]->clone());

        KineticMapArtist  ->setFunction(kinetic_func);
        //kinetic_map_artist  ->setColorMap(Graphics::ColorMaps["afmhot"]->brg().clone());

        GradMapArtist     ->setFunction(grad_func);
        //grad_map_artist     ->setColorMap(Graphics::ColorMaps["afmhot"]->brg().brg().clone());

        PotentialMapArtist->setFunction(potential_func);
        //potential_map_artist->setColorMap(Graphics::ColorMaps["afmhot"]->bgr().clone());

        {
            auto dfdx = DynamicPointerCast<R2toR::FNumericFunction>(f->diff(0));
            IN dfdx_data = dfdx->getSpace().getHostData(true);
            IN dfdt_data = dfdt->getSpace().getHostData(true);

            OUT mf_data = momentum_flow_func->getSpace().getHostData(true);

            fix data_size = dfdx_data.size();
            for(auto i=0; i<data_size; ++i)
                mf_data[i] = - dfdt_data[i] * dfdx_data[i];

            MomentumFlowMapArtist->setFunction(momentum_flow_func);
            // momentum_flow_map_artist->setColorMap(Graphics::ColorMaps["cmocean:curl"]->clone());
        }

        TotalEnergiesWindow->ReviewGraphRanges();
        FullHistoriesWindow->ReviewGraphRanges();
    }

    void KGRtoR::EnergyViewer_KG::NotifyBecameVisible() {
        Viewer::NotifyBecameVisible();

        if(areFunctionsConsistent()) UpdateEnergy();
    }

    Str KGRtoR::EnergyViewer_KG::GetName() const {
        return "[KG] Stress-energy viewer";
    }
} // Slab::Models