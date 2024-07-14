//
// Created by joao on 7/10/24.
//

#include "EnergyViewer_KG.h"
#include "Math/Function/RtoR/Operations/FromR2toRNumeric.h"
#include "Graphics/Graph/Plotter.h"
#include "Graphics/Graph/PlotThemeManager.h"

namespace Slab::Models {

    KGRtoR::EnergyViewer_KG::EnergyViewer_KG(const Pointer<Graphics::GUIWindow> &gui_window)
    : KGViewer(gui_window)
    {
        addWindow(temperatures_window);
        addWindow(total_energies_window);
        addWindow(full_histories_window, true);

        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
        energy_history_artist    = Graphics::Plotter::AddPointSet(total_energies_window, energy_history,
                                                                  style, "e", true);
        kinetic_history_artist   = Graphics::Plotter::AddPointSet(total_energies_window, kinetic_history,
                                                                  style, "½(𝜕ₜf)²", true);
        grad_history_artist      = Graphics::Plotter::AddPointSet(total_energies_window, grad_history,
                                                                  style, "½(𝜕ₓf)²", true);
        potential_history_artist = Graphics::Plotter::AddPointSet(total_energies_window, potential_history,
                                                                  style, "|f|", true);

        f_artist         = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "ϕ(x,t)");
        dfdt_artist      = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "T¹¹=𝜕ₜϕ");

        energy_map_artist    = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "T⁰⁰=e(x,t)");
        kinetic_map_artist   = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "k(x,t)");
        grad_map_artist      = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "g(x,t)");
        potential_map_artist = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "v(x,t)");

        momentum_flow_map_artist = Graphics::Plotter::AddR2toRFunction(full_histories_window, nullptr, "T⁰¹=-𝜕ₓϕ𝜕ₜϕ");

        energy_map_artist   ->setAffectGraphRanges(true);
        kinetic_map_artist  ->setAffectGraphRanges(true);
        grad_map_artist     ->setAffectGraphRanges(true);
        potential_map_artist->setAffectGraphRanges(true);


    }

    void KGRtoR::EnergyViewer_KG::setFunction(Pointer<Math::R2toR::NumericFunction> function) {
        Viewer::setFunction(function);

        f_artist->setFunction(getFunction());
        f_artist->setColorMap(Graphics::ColorMaps["BrBG"]->inverse().clone());
    }

    void KGRtoR::EnergyViewer_KG::setFunctionDerivative(FuncPointer pointer) {
        KGViewer::setFunctionDerivative(pointer);

        dfdt_artist->setFunction(pointer);
        dfdt_artist->setColorMap(Graphics::ColorMaps["BrBG"]->brg().inverse().clone());

        if(isVisible() && areFunctionsConsistent()) updateEnergy();
    }

    void KGRtoR::EnergyViewer_KG::updateEnergy() {
        if(!areFunctionsConsistent()) return;

        auto f = getFunction();
        auto dfdt = getFunctionDerivative();

        auto H = getHamiltonian();

        auto energy_func         = DynamicPointerCast<R2toR::NumericFunction>(f->Clone());
        auto kinetic_func        = DynamicPointerCast<R2toR::NumericFunction>(f->Clone());
        auto grad_func           = DynamicPointerCast<R2toR::NumericFunction>(f->Clone());
        auto potential_func      = DynamicPointerCast<R2toR::NumericFunction>(f->Clone());

        auto momentum_flow_func  = DynamicPointerCast<R2toR::NumericFunction>(f->Clone());

        energy_history->clear();
        kinetic_history->clear();
        grad_history->clear();
        potential_history->clear();

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
                energy_history   ->addPoint(t, H.getTotalEnergy());
                kinetic_history  ->addPoint(t, H.getTotalKineticEnergy());
                grad_history     ->addPoint(t, H.getTotalGradientEnergy());
                potential_history->addPoint(t, H.getTotalPotentialEnergy());
            }

            {
                const
                double *in = &H.getEnergyDensity()->getSpace().getHostData(true)[0];
                double *out = &energy_func->At(0, j);

                memcpy(out, in, sizeof(Real) * N);
            }

            {
                const
                double *in = &H.getKineticDensity()->getSpace().getHostData(true)[0];
                double *out = &kinetic_func->At(0, j);

                memcpy(out, in, sizeof(Real) * N);
            }

            {
                const
                double *in = &H.getGradientDensity()->getSpace().getHostData(true)[0];
                double *out = &grad_func->At(0, j);

                memcpy(out, in, sizeof(Real) * N);
            }

            {
                const
                double *in = &H.getPotentialDensity()->getSpace().getHostData(true)[0];
                double *out = &potential_func->At(0, j);

                memcpy(out, in, sizeof(Real) * N);
            }
        }

        energy_map_artist   ->setFunction(energy_func);
        energy_map_artist   ->setColorMap(Graphics::ColorMaps["afmhot"]->clone());

        kinetic_map_artist  ->setFunction(kinetic_func);
        kinetic_map_artist  ->setColorMap(Graphics::ColorMaps["afmhot"]->brg().clone());

        grad_map_artist     ->setFunction(grad_func);
        grad_map_artist     ->setColorMap(Graphics::ColorMaps["afmhot"]->brg().brg().clone());

        potential_map_artist->setFunction(potential_func);
        potential_map_artist->setColorMap(Graphics::ColorMaps["afmhot"]->bgr().clone());

        {
            auto dfdx = DynamicPointerCast<R2toR::NumericFunction>(f->diff(0));
            IN dfdx_data = dfdx->getSpace().getHostData(true);
            IN dfdt_data = dfdt->getSpace().getHostData(true);

            OUT mf_data = momentum_flow_func->getSpace().getHostData(true);

            fix data_size = dfdx_data.size();
            for(auto i=0; i<data_size; ++i)
                mf_data[i] = - dfdt_data[i] * dfdx_data[i];

            momentum_flow_map_artist->setFunction(momentum_flow_func);
            momentum_flow_map_artist->setColorMap(Graphics::ColorMaps["cmocean:curl"]->clone());
        }

        total_energies_window->reviewGraphRanges();
        full_histories_window->reviewGraphRanges();
    }

    void KGRtoR::EnergyViewer_KG::notifyBecameVisible() {
        Viewer::notifyBecameVisible();

        if(areFunctionsConsistent()) updateEnergy();
    }
} // Slab::Models